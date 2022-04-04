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
#include "../../Components/Components.hpp"
#include "../../Entitylist/EntityList.hpp"

#include "../Renderer3D/Renderer3D.hpp"
#include "../Skybox/Skybox.hpp"
#include "../PostProcessing/PostProcessing.hpp"

namespace Pine
{

	class CRenderManager : public IRenderManager
	{
	private:

		RenderingContext* g_RenderingContext = nullptr;
		RenderCallback g_RenderingCallback = nullptr;

		bool VerifyRenderingContext( RenderingContext* context )
		{
			if ( !context )
				return false;

			// I would check for a camera being available, but right now I want the PreRender callback to be called
			// so this function looks very silly.

			return true;
		}

		ImGuiContext* g_RenderingImGuiContext = nullptr;

	public:

		void Render( ) override
		{
            Timer totalRenderTime;

			if ( !VerifyRenderingContext( g_RenderingContext ) )
			{
				return;
			}

			// This is fucking retarded, please write a proper solution.
			// Everything beyond this point should also respect the render context's
			// target size, but it won't so keep that in mind.

            g_RenderingContext->m_Width = 1920;
			g_RenderingContext->m_Height = 1080;

			// NOTE: The reason why this is annoying is because of the post processing frame buffer's size, something the engine won't dynamically update at this moment.
			// to fix this temporary just update that and set the rendering context's size accordingly.

			if ( g_RenderingCallback )
				g_RenderingCallback( RenderStage::PreRender );

			PrepareSceneRendering( );

			// Reset stats
			g_RenderingContext->m_DrawCalls = 0;
			g_RenderingContext->m_EntitySortTime = 0;
			g_RenderingContext->m_EntityRenderTime = 0;
			g_RenderingContext->m_PostProcessingTime = 0;
            g_RenderingContext->m_ComponentUpdateTime.fill(0);

			if ( g_RenderingContext->m_Camera == nullptr )
				return;

            Timer entityUpdateTime;

			// Call "OnRender" for each component
			for ( int i = 0; i < Components->GetComponentTypeCount( ); i++ )
			{
                Timer componentUpdateTime;

				for ( int j = 0; j < Components->GetComponentCount( static_cast< ComponentType >( i ) ); j++ )
				{
					if ( const auto component = Components->GetComponent( static_cast< ComponentType >( i ), j ) )
                    {
                        component->OnRender( );
                    }
				}

                componentUpdateTime.Stop();
                g_RenderingContext->m_ComponentUpdateTime[i] = componentUpdateTime.GetElapsedTimeInMs();
			}

            entityUpdateTime.Stop();

            // Sort entities with the blend rendering mode in a different map
			std::unordered_map<Model*, std::vector<ModelRenderer*>> renderBatch;
			std::unordered_map<Model*, std::vector<ModelRenderer*>> renderBatchBlend;

			std::vector<Light*> lights;
			std::vector<TerrainRenderer*> terrainRenderers;

			auto componentSanityCheck = [ ] ( const Pine::IComponent* component )
			{
				return component && component->GetActive( ) && component->GetParent( )->GetActive( );
			};

			Timer entitySortTimer;

			// Model Renderer

			for ( int i = 0; i < Components->GetComponentCount( ComponentType::ModelRenderer ); i++ )
			{
				const auto modelRenderer = dynamic_cast< ModelRenderer* >( Components->GetComponent( ComponentType::ModelRenderer, i ) );

				if ( componentSanityCheck( modelRenderer ) && modelRenderer->GetModel( ) )
				{
					auto blendRenderingMode = false;

					// Check if any of the material used within this model has a different rendering mode.
					for ( const auto mesh : modelRenderer->GetModel( )->GetMeshList( ) )
						if ( mesh->GetMaterial( ) && mesh->GetMaterial( )->GetRenderingMode( ) == MatRenderingMode::Transparent )
							blendRenderingMode = true;
					if ( const auto ov = modelRenderer->GetMaterialOverride(  ) )
						if ( ov->GetRenderingMode(  ) == MatRenderingMode::Transparent )
							blendRenderingMode = true;

					if ( blendRenderingMode )
						renderBatchBlend[ modelRenderer->GetModel( ) ].push_back( modelRenderer );
					else
						renderBatch[ modelRenderer->GetModel( ) ].push_back( modelRenderer );
				}
			}

			// Light

			for ( int i = 0; i < Components->GetComponentCount( ComponentType::Light ); i++ )
			{
				const auto light = dynamic_cast< Light* >( Components->GetComponent( ComponentType::Light, i ) );

				if ( componentSanityCheck( light ) )
				{
					lights.push_back( light );
				}
			}

			// Terrain Renderer

			for ( int i = 0; i < Components->GetComponentCount( ComponentType::TerrainRenderer ); i++ )
			{
				const auto terrainRenderer = dynamic_cast< TerrainRenderer* >( Components->GetComponent( ComponentType::TerrainRenderer, i ) );

				if ( componentSanityCheck( terrainRenderer ) )
				{
					terrainRenderers.push_back( terrainRenderer );
				}
			}


			// Prepare the light data before uploading it to the GPU
			Renderer3D->ResetLightData( );

			for ( const auto light : lights )
			{
				Renderer3D->PrepareLightData( light );
			}

			Renderer3D->UploadLightData( );

			Renderer3D->PrepareMeshRendering( );

            entitySortTimer.Stop( );

			Timer entityRenderTime;

			// Render Terrain Chunks

			for ( const auto terrainRenderer : terrainRenderers )
			{
				const auto terrain = terrainRenderer->GetTerrain( );

				if ( !terrain )
					continue;

				Renderer3D->PrepareTerrain( terrain );

				for ( auto& chunk : terrain->GetChunks( ) )
				{
					Renderer3D->RenderTerrainChunk( chunk );
					g_RenderingContext->m_DrawCalls++;
				}
			}

			// Render entities
			
			for ( auto& renderItem : renderBatch )
			{
				for ( auto& mesh : renderItem.first->GetMeshList( ) )
				{
					Renderer3D->PrepareMesh( mesh );

					for ( const auto modelRenderer : renderItem.second )
					{
						const auto entity = modelRenderer->GetParent( );

						bool restoreMesh = false;

						// Doing shit like this will slow down all rendering but whatever,
						// it's the user's fault rendering stuff like this :-)
						if ( modelRenderer->GetMaterialOverride( ) != nullptr || modelRenderer->GetOverridingStencilBuffer( ) )
						{
							Renderer3D->PrepareMesh( mesh, modelRenderer->GetMaterialOverride( ), modelRenderer->GetOverridedStencilBufferMask( ) );

							modelRenderer->OverrideStencilBuffer( false, 0x00 );

							restoreMesh = true;
						}

						Renderer3D->RenderMesh( entity->GetTransform( )->GetTransformationMatrix( ) );

						if ( restoreMesh )
						{
							Renderer3D->PrepareMesh( mesh );
						}
					}
				}
			}

			// Reset some properties that may have been set.
			Renderer3D->SetWireframeMode( false );
			Renderer3D->SetBackfaceCulling( true );
			Renderer3D->SetBlending( false );

			Skybox->Render( );

			if ( g_RenderingCallback )
				g_RenderingCallback( RenderStage::PostRenderEntities );

			entityRenderTime.Stop( );

			Timer postProcessingTime;

			FinishSceneRendering( );

			g_RenderingContext->m_DrawCalls++;

			postProcessingTime.Stop( );
            totalRenderTime.Stop( );

            g_RenderingContext->m_EntityUpdateTime = entityUpdateTime.GetElapsedTimeInMs( );
            g_RenderingContext->m_EntitySortTime = entitySortTimer.GetElapsedTimeInMs( );
			g_RenderingContext->m_EntityRenderTime = entityRenderTime.GetElapsedTimeInMs( );
			g_RenderingContext->m_PostProcessingTime = postProcessingTime.GetElapsedTimeInMs( );
            g_RenderingContext->m_TotalRenderTime = totalRenderTime.GetElapsedTimeInMs();

			if ( g_RenderingCallback )
				g_RenderingCallback( RenderStage::PostRender );
        }

		void SetRenderingContext( RenderingContext* renderingContext ) override
		{
			g_RenderingContext = renderingContext;
		}

		RenderingContext* GetRenderingContext( ) override
		{
			return g_RenderingContext;
		}

		void SetRenderingCallback( const RenderCallback fn ) override
		{
			g_RenderingCallback = fn;
		}

		void PrepareSceneRendering( ) override
		{
			PostProcessing->GetRenderBuffer( )->Bind( );

			// Clear the buffers
			glClearColor( g_RenderingContext->m_ClearColor.r, g_RenderingContext->m_ClearColor.g, g_RenderingContext->m_ClearColor.b, 1.f );
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

			// Reset the viewport size.
			glViewport( 0, 0, g_RenderingContext->m_Width, g_RenderingContext->m_Height );

			// Enable depth test, stencil testing and face culling
			glEnable( GL_DEPTH_TEST );
			glEnable( GL_STENCIL_TEST );
			glEnable( GL_CULL_FACE );

			glStencilOp( GL_KEEP, GL_KEEP, GL_REPLACE );
			glCullFace( GL_BACK );

			if ( g_RenderingContext->m_Camera != nullptr )
				Renderer3D->UploadCameraData( g_RenderingContext->m_Camera );

			Renderer3D->SetShader( nullptr );
			Renderer3D->SetWireframeMode( false );
			Renderer3D->SetBackfaceCulling( true );
		}

		void FinishSceneRendering( ) override
		{
			const bool hasFrameBuffer = g_RenderingContext->m_FrameBuffer != nullptr;

			// Setup frame buffer
			if ( hasFrameBuffer )
			{
				g_RenderingContext->m_FrameBuffer->Bind( );

				// Override rendering context's size variables.
				if ( g_RenderingContext->m_AutoUpdateSize )
				{
					g_RenderingContext->m_Width = g_RenderingContext->m_FrameBuffer->GetWidth( );
					g_RenderingContext->m_Height = g_RenderingContext->m_FrameBuffer->GetHeight( );
				}
			}
			else
			{
				glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			}

			PostProcessing->Render( );
		}

		void Setup( ) override
		{
			Log->Debug( "Pine::RenderManager->Setup( )" );

			g_RenderingImGuiContext = ImGui::CreateContext( );
		}

	};

	IRenderManager* CreateRenderManagerInterface( )
	{
		return new CRenderManager;
	}

}
