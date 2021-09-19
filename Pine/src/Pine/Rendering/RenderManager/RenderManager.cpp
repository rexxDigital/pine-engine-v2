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
	// Everything beyond this point should also respect the render context's
	// target size, but it won't so keep that in mind.
	//g_RenderingContext->m_Width = 1600;
	//g_RenderingContext->m_Height = 900;

	// NOTE: The reason why this is annoying is because of the post processing frame buffer's size, something the engine won't dynamically update at this moment.
	// to fix this temporary just update that and set the rendering context's size accordingly.

	if ( g_PreRenderingCallback ) {
		g_PreRenderingCallback( );
	}

	PrepareSceneRendering( );
	
	// Reset stats
	g_RenderingContext->m_DrawCalls = 0;
	g_RenderingContext->m_EntitySortTime = 0;
	g_RenderingContext->m_EntityRenderTime = 0;
	g_RenderingContext->m_PostProcessingTime = 0;
	
	if ( g_RenderingContext->m_Camera == nullptr )
		return;

	// Better to keep this on the stack, since we want it empty the next frame anyway.
	// Also quicker! UPDATE: Why am I writing this useless nonsense?
	std::unordered_map<Pine::Model*, std::vector<Pine::Entity*>> renderBatch;
	std::vector<Pine::Light*> lights;
	std::vector<Pine::TerrainRenderer*> terrainRenderers;

	auto processEntity = [ & ] ( Pine::Entity* entity ) {
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
			processEntity( child );

		processEntity( entity );
	}

	entitySortTimer.Stop( );

	// Prepare the light data before uploading it to the GPU:
	Renderer3D::ResetLightData( );
	
	for ( auto light : lights ) {
		Renderer3D::PrepareLightData( light );
	}

	Renderer3D::UploadLightData( );

	Renderer3D::PrepareMeshRendering( );

	Timer entityRenderTime;

	// Render Terrain Chunks

	for ( auto terrainRenderer : terrainRenderers ) {
		const auto terrain = terrainRenderer->GetTerrain( );

		if ( !terrain )
			continue;

		Renderer3D::PrepareTerrain( terrain );

		for ( auto& chunk : terrain->GetChunks( ) )
		{
			Renderer3D::RenderTerrainChunk( chunk );
			g_RenderingContext->m_DrawCalls++;
		}
	}

	// Render Normal Entities

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

	Timer postProcessingTime;

	FinishSceneRendering( );

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

void Pine::RenderManager::PrepareSceneRendering( )
{
	PostProcessing::GetRenderBuffer( )->Bind( );

	// Clear the buffers
	glClearColor( 0.f, 0.f, 0.f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	// Reset the viewport size.
	glViewport( 0, 0, g_RenderingContext->m_Width, g_RenderingContext->m_Height );

	// Enable depth test just in case.
	glEnable( GL_DEPTH_TEST );

	if ( g_RenderingContext->m_Camera != nullptr )
		Renderer3D::UploadCameraData( g_RenderingContext->m_Camera );
}

void Pine::RenderManager::FinishSceneRendering(  )
{
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
	
	PostProcessing::Render( );
}

void Pine::RenderManager::Setup( ) {
	Log::Debug( "Pine::RenderManager::Setup()" );

	g_RenderingImGuiContext = ImGui::CreateContext( );
}
