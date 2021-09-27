#include "RenderManager.hpp"
#include <unordered_map>
#include <vector>
#include <chrono>
#include <GL/glew.h>

#include "../../Components/ModelRenderer/ModelRenderer.hpp"
#include "../../Components/Light/Light.hpp"
#include "../../Components/TerrainRenderer/TerrainRenderer.hpp"

#include "../../Core/Log/Log.hpp"
#include "../../Core/Timer/Timer.hpp"

#include "../../../ImGui/imgui_impl_opengl3.h"

#include "../../Assets/Terrain/Terrain.hpp"
#include "../../Entitylist/EntityList.hpp"

#include "../Renderer3D/Renderer3D.hpp"
#include "../Skybox/Skybox.hpp"
#include "../PostProcessing/PostProcessing.hpp"

namespace {

	Pine::RenderingContext* g_RenderingContext = nullptr;

	Pine::RenderCallback g_RenderingCallback = nullptr;

	bool VerifyRenderingContext( Pine::RenderingContext* context ) {
		if ( !context )
			return false;

		// I would check for a camera being available, but right now I want the PreRender callback to be called
		// so this function looks very silly.

		return true;
	}

	ImGuiContext* g_RenderingImGuiContext = nullptr;

}

void Pine::RenderManager::Render( ) {
	if ( !VerifyRenderingContext( g_RenderingContext ) ) {
		return;
	}

	// This is fucking retarded, please write a proper solution.
	// Everything beyond this point should also respect the render context's
	// target size, but it won't so keep that in mind.
	g_RenderingContext->m_Width = 1600;
	g_RenderingContext->m_Height = 900;

	// NOTE: The reason why this is annoying is because of the post processing frame buffer's size, something the engine won't dynamically update at this moment.
	// to fix this temporary just update that and set the rendering context's size accordingly.

	if ( g_RenderingCallback ) {
		g_RenderingCallback( 0 );
	}

	PrepareSceneRendering( );

	// Reset stats
	g_RenderingContext->m_DrawCalls = 0;
	g_RenderingContext->m_EntitySortTime = 0;
	g_RenderingContext->m_EntityRenderTime = 0;
	g_RenderingContext->m_PostProcessingTime = 0;

	if ( g_RenderingContext->m_Camera == nullptr )
		return;

	std::unordered_map<Pine::Model*, std::vector<Pine::ModelRenderer*>> renderBatch;
	std::vector<Pine::Light*> lights;
	std::vector<Pine::TerrainRenderer*> terrainRenderers;

	auto processEntity = [ & ] ( const Pine::Entity* entity ) {
		for ( auto& component : entity->GetComponents( ) ) {
			if ( !component->GetActive( ) ) {
				continue;
			}

			component->OnRender( );

			if ( component->GetType( ) == EComponentType::ModelRenderer ) {
				const auto modelRenderer = dynamic_cast<Pine::ModelRenderer*>( component );
				auto model = modelRenderer->GetTargetModel( );

				if ( model != nullptr ) {
					renderBatch[ model ].push_back( modelRenderer );
				}
			}
			else if ( component->GetType( ) == EComponentType::Light ) {
				lights.push_back( dynamic_cast<Pine::Light*>( component ) );
			}
			else if ( component->GetType( ) == EComponentType::TerrainRenderer ) {
				terrainRenderers.push_back( dynamic_cast<Pine::TerrainRenderer*>( component ) );
			}
		}
	};

	Timer entitySortTimer;

	for ( auto& entity : EntityList::GetEntities( ) ) {
		if ( !entity.GetActive( ) ) {
			continue;
		}

		for ( const auto child : entity.GetChildren( ) )
			processEntity( child );

		processEntity( &entity );
	}

	entitySortTimer.Stop( );

	// Prepare the light data before uploading it to the GPU:
	Renderer3D::ResetLightData( );

	for ( const auto light : lights ) {
		Renderer3D::PrepareLightData( light );
	}

	Renderer3D::UploadLightData( );

	Renderer3D::PrepareMeshRendering( );

	Timer entityRenderTime;

	// Render Terrain Chunks

	for ( const auto terrainRenderer : terrainRenderers ) {
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

			for ( const auto modelRenderer : renderItem.second ) {
				const auto entity = modelRenderer->GetParent( );

				bool restoreMesh = false;

				// Doing shit like this will slow down all rendering but whatever,
				// it's the user's fault rendering stuff like this :-)
				if ( modelRenderer->GetMaterialOverride( ) != nullptr || modelRenderer->GetOverridingStencilBuffer( ) )
				{
					Renderer3D::PrepareMesh( mesh, modelRenderer->GetMaterialOverride( ), modelRenderer->GetOverridedStencilBufferMask( ) );

					modelRenderer->OverrideStencilBuffer( false, 0x00 );

					restoreMesh = true;
				}

				Renderer3D::RenderMesh( entity->GetTransform( )->GetTransformationMatrix( ) );

				if ( restoreMesh )
				{
					Renderer3D::PrepareMesh( mesh );
				}
			}
		}
	}

	Skybox::Render( );

	if ( g_RenderingCallback ) {
		g_RenderingCallback( 1 );
	}

	entityRenderTime.Stop( );

	Timer postProcessingTime;

	FinishSceneRendering( );

	g_RenderingContext->m_DrawCalls++;

	postProcessingTime.Stop( );

	g_RenderingContext->m_EntitySortTime = entitySortTimer.GetElapsedTimeInMs( );
	g_RenderingContext->m_EntityRenderTime = entityRenderTime.GetElapsedTimeInMs( );
	g_RenderingContext->m_PostProcessingTime = postProcessingTime.GetElapsedTimeInMs( );

	if ( g_RenderingCallback ) {
		g_RenderingCallback( 2 );
	}
}

void Pine::RenderManager::SetRenderingContext( RenderingContext* renderingContext ) {
	g_RenderingContext = renderingContext;
}

Pine::RenderingContext* Pine::RenderManager::GetRenderingContext( ) {
	return g_RenderingContext;
}

void Pine::RenderManager::SetRenderingCallback( RenderCallback fn ) {
	g_RenderingCallback = fn;
}

void Pine::RenderManager::PrepareSceneRendering( )
{
	PostProcessing::GetRenderBuffer( )->Bind( );

	// Clear the buffers
	glClearColor( 0.f, 0.f, 0.f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

	// Reset the viewport size.
	glViewport( 0, 0, g_RenderingContext->m_Width, g_RenderingContext->m_Height );

	// Enable depth test just in case.
	glEnable( GL_DEPTH_TEST );
	glEnable( GL_STENCIL_TEST );

	glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );

	if ( g_RenderingContext->m_Camera != nullptr )
		Renderer3D::UploadCameraData( g_RenderingContext->m_Camera );
}

void Pine::RenderManager::FinishSceneRendering( )
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
	Log::Debug( "Pine::RenderManager::Setup( )" );

	g_RenderingImGuiContext = ImGui::CreateContext( );
}
