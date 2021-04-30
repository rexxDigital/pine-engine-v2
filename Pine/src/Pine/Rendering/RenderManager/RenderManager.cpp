#include "RenderManager.hpp"
#include <unordered_map>
#include <vector>
#include <chrono>
#include "../../Components/ModelRenderer/ModelRenderer.hpp"
#include "../../Entitylist/EntityList.hpp"
#include "../Renderer3D/Renderer3D.hpp"
#include "../../Components/Camera/Camera.hpp"
#include "../UniformBuffers/UniformBuffers.hpp"
#include "../../Components/Light/Light.hpp"
#include "../../Core/Log/Log.hpp"
#include "../../Core/Window/Window.hpp"
#include "../Skybox/Skybox.hpp"
#include <GL/glew.h>


#include "../../../ImGui/imgui_impl_glfw.h"
#include "../../../ImGui/imgui_impl_opengl3.h"
#include "../../Assets/Terrain/Terrain.hpp"
#include "../../Components/TerrainRenderer/TerrainRenderer.hpp"
#include "../../Core/Timer/Timer.hpp"
#include "../PostProcessing/PostProcessing.hpp"

namespace {

	Pine::RenderingContext* g_RenderingContext = nullptr;

	Pine::RenderCallback g_PreRenderingCallback = nullptr;
	Pine::RenderCallback g_PostRenderingCallback = nullptr;

	bool VerifyRenderingContext( Pine::RenderingContext* context ) {
		if ( !context )
			return false;

		// I would check for a camera being available, but i want the PreRender callback to be called.
		
		return true;
	}

	ImGuiContext* g_RenderingImGuiContext = nullptr;

}

void Pine::RenderManager::Run( ) {
	if ( !VerifyRenderingContext( g_RenderingContext ) ) {
		return;
	}

	// This is fucking retarded, please write a proper solution.
	// Everything beyond this point, should also respect the render context's
	// target size, but it won't so keep that in mind.
	g_RenderingContext->m_Width = 1600;
	g_RenderingContext->m_Height = 900;

	// Reset stats
	g_RenderingContext->m_DrawCalls = 0;
	g_RenderingContext->m_EntitySortTime = 0;
	g_RenderingContext->m_EntityRenderTime = 0;
	g_RenderingContext->m_PostProcessingTime = 0;

	PostProcessing::GetRenderBuffer( )->Bind( );

	// Clear the buffers
	glClearColor( 0.f, 0.f, 0.f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Reset the viewport size.
	glViewport( 0, 0, g_RenderingContext->m_Width, g_RenderingContext->m_Height );

	// Enable depth test just in case.
	glEnable( GL_DEPTH_TEST );

	if ( g_PreRenderingCallback ) {
		g_PreRenderingCallback( );
	}

	if ( g_RenderingContext->m_Camera == nullptr )
		return;

	// Better to keep this on the stack, since we want it empty the next frame anyway.
	// Also quicker! UPDATE: Why am I writing this useless nonsense?
	std::unordered_map<Pine::Model*, std::vector<Pine::Entity*>> renderBatch;
	std::vector<Pine::Light*> lights;
	std::vector<Pine::TerrainRenderer*> terrainRenderers;

	auto renderEntity = [ & ]( Pine::Entity* entity ) {
		for ( auto& component : entity->GetComponents( ) ) {
			if ( !component->GetActive( ) ) {
				continue;
			}

			component->OnRender( );

			if ( component->GetType( ) == EComponentType::ModelRenderer ) {
				auto modelRenderer = dynamic_cast< Pine::ModelRenderer* >( component );
				auto model = modelRenderer->GetTargetModel( );

				if ( model != nullptr ) {
					renderBatch[ model ].push_back( entity );
				}
			}
			else if ( component->GetType( ) == EComponentType::Light ) {
				lights.push_back( dynamic_cast< Pine::Light* >( component ) );
			}
			else if ( component->GetType( ) == EComponentType::TerrainRenderer ) {
				terrainRenderers.push_back( dynamic_cast< Pine::TerrainRenderer* >( component ) );
			}
		}
	};

	Timer entitySortTimer;

	for ( auto& entity : EntityList::GetEntities( ) ) {
		if ( !entity->GetActive( ) ) {
			continue;
		}

		for ( auto child : entity->GetChildren( ) )
			renderEntity( child );

		renderEntity( entity );
	}

	entitySortTimer.Stop( );

	// Prepare data
	UniformBuffers::GetMatrixBufferData( )->ProjectionMatrix = g_RenderingContext->m_Camera->GetProjectionMatrix( );
	UniformBuffers::GetMatrixBufferData( )->ViewMatrix = g_RenderingContext->m_Camera->GetViewMatrix( );

	for ( int i = 0; i < 4; i++ ) {
		UniformBuffers::GetLightsBufferData( )->lights[ i ].color = glm::vec3( 0.f, 0.f, 0.f );
		UniformBuffers::GetLightsBufferData( )->lights[ i ].position = glm::vec3( 0.f, 0.f, 0.f );
		UniformBuffers::GetLightsBufferData( )->lights[ i ].attenuation = glm::vec3( 0.f, 0.f, 0.f );
	}

	int processedLights = 1;
	for ( auto light : lights ) {
		if ( processedLights >= 4 ) {
			Log::Warning( "Too many dynamic lights in level." );
			break;
		}

		if ( light->GetLightType( ) == Pine::ELightType::Directional ) {
			UniformBuffers::GetLightsBufferData( )->lights[ 0 ].position = light->GetParent( )->GetTransform( )->Position;
			UniformBuffers::GetLightsBufferData( )->lights[ 0 ].rotation = glm::normalize( light->GetParent( )->GetTransform( )->Rotation );
			UniformBuffers::GetLightsBufferData( )->lights[ 0 ].color = light->GetLightColor( );
			UniformBuffers::GetLightsBufferData( )->lights[ 0 ].attenuation = light->GetAttenuation( );
		}
		else {
			UniformBuffers::GetLightsBufferData( )->lights[ processedLights ].position = light->GetParent( )->GetTransform( )->Position;
			UniformBuffers::GetLightsBufferData( )->lights[ processedLights ].rotation = glm::normalize( light->GetParent( )->GetTransform( )->Rotation );
			UniformBuffers::GetLightsBufferData( )->lights[ processedLights ].color = light->GetLightColor( );
			UniformBuffers::GetLightsBufferData( )->lights[ processedLights ].attenuation = light->GetAttenuation( );
		}

		processedLights++;
	}

	// Upload to the uniform buffer
	UniformBuffers::GetMatrixUniformBuffer( )->Bind( );
	UniformBuffers::GetMatrixUniformBuffer( )->UploadData( 0, sizeof( UniformBuffers::MatrixBufferData_t ), UniformBuffers::GetMatrixBufferData( ) );

	UniformBuffers::GetLightsUniformBuffer( )->Bind( );
	UniformBuffers::GetLightsUniformBuffer( )->UploadData( 0, sizeof( UniformBuffers::LightBufferData_t ), UniformBuffers::GetLightsBufferData( ) );

	UniformBuffers::GetMaterialUniformBuffer( )->Bind( );

	Timer entityRenderTime;
	
	/* Render Terrain Chunks */
	
	for ( auto terrainRenderer : terrainRenderers ) {
		const auto terrain = terrainRenderer->GetTerrain( );

		if ( !terrain )
			continue;

		Renderer3D::PrepareTerrain( terrain );
		
		for ( auto& chunk : terrain->GetChunks(  ) )
		{
			Renderer3D::RenderTerrainChunk( chunk );
			g_RenderingContext->m_DrawCalls++;
		}
	}

	/* Render Normal Entities */

	for ( auto& renderItem : renderBatch ) {
		for ( auto& mesh : renderItem.first->GetMeshList( ) ) {
			Renderer3D::PrepareMesh( mesh );

			for ( auto entity : renderItem.second ) {
				Renderer3D::RenderMesh( entity->GetTransform( )->GetTransformationMatrix( ) );
			}
		}
	}

	Skybox::Render( );

	entityRenderTime.Stop( );
	
	const bool hasFrameBuffer = g_RenderingContext->m_FrameBuffer != nullptr;

	// Setup frame buffer
	if ( hasFrameBuffer ) {
		g_RenderingContext->m_FrameBuffer->Bind( );

		// Override rendering context's size variables.
		if ( g_RenderingContext->m_AutoUpdateSize ) {
			g_RenderingContext->m_Width = g_RenderingContext->m_FrameBuffer->GetWidth( );
			g_RenderingContext->m_Height = g_RenderingContext->m_FrameBuffer->GetHeight( );
		}
	}
	else {
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
	}

	Timer postProcessingTime;
	
	PostProcessing::Render( );

	g_RenderingContext->m_DrawCalls++;

	postProcessingTime.Stop( );

	g_RenderingContext->m_EntitySortTime = entitySortTimer.GetElapsedTimeInMs( );
	g_RenderingContext->m_EntityRenderTime = entityRenderTime.GetElapsedTimeInMs( );
	g_RenderingContext->m_PostProcessingTime = postProcessingTime.GetElapsedTimeInMs( );

	if ( g_PostRenderingCallback ) {
		g_PostRenderingCallback( );
	}
}

void Pine::RenderManager::SetRenderingContext( RenderingContext* renderingContext ) {
	g_RenderingContext = renderingContext;
}

Pine::RenderingContext* Pine::RenderManager::GetRenderingContext( ) {
	return g_RenderingContext;
}

void Pine::RenderManager::SetPreRenderingCallback( RenderCallback fn ) {
	g_PreRenderingCallback = fn;
}

void Pine::RenderManager::SetPostRenderingCallback( RenderCallback fn ) {
	g_PostRenderingCallback = fn;
}

void Pine::RenderManager::Setup( ) {
	Log::Debug( "Pine::RenderManager::Setup()" );
	
	g_RenderingImGuiContext = ImGui::CreateContext( );
}
