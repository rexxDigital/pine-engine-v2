#include "RenderingHandler.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <Pine/Pine.hpp>
#include <Pine/Rendering/RenderManager/RenderManager.hpp>
#include <Pine/OpenGL/FrameBuffer/FrameBuffer.hpp>

#include <ImGui/imgui.h>

#include "Editor/EditorEntity/EditorEntity.hpp"
#include "Editor/Gui/Gui.hpp"
#include "Pine/Components/ModelRenderer/ModelRenderer.hpp"

namespace
{
	Pine::FrameBuffer* g_RenderingFrameBuffer = nullptr;

	Pine::Camera* g_BackupCamera = nullptr;
	Pine::Camera* g_EditorCamera = nullptr;

	Pine::Shader* g_OutlineShader = nullptr;

	// Render an outline on the selected model
	void RenderHighlightedEntity( )
	{
		if ( Editor::Gui::Globals::SelectedEntityPtrs.empty( ) )
			return;

		const auto entity = Editor::Gui::Globals::SelectedEntityPtrs[ 0 ];
		const auto modelRenderer = entity->GetComponent<Pine::ModelRenderer>( );

		if ( modelRenderer && modelRenderer->GetTargetModel( ) )
		{
			Pine::Renderer3D->SetStencilFunction( 0x0205, 0xFF );
			Pine::Renderer3D->SetStencilMask( 0x00 );
			Pine::Renderer3D->SetDepthTesting( false );

			if ( const auto model = modelRenderer->GetTargetModel( ) )
			{
				for ( const auto mesh : model->GetMeshList( ) )
				{
					Pine::Renderer3D->PrepareMesh( mesh );
					Pine::Renderer3D->SetShader( g_OutlineShader );

					g_OutlineShader->Use( );

					glm::mat4 mat = entity->GetTransform( )->GetTransformationMatrix( );

					mat = glm::scale( mat, glm::vec3( 1.04f, 1.04f, 1.04f ) );

					Pine::Renderer3D->RenderMesh( mat );
				}
			}

			Pine::Renderer3D->SetStencilMask( 0xFF );
			Pine::Renderer3D->SetStencilFunction( 0x0207, 0xFF );
			Pine::Renderer3D->SetDepthTesting( true );
		}
	}

	void OnPreRender( )
	{
		// Render with level editor entity camera
		if ( Editor::Gui::Globals::IsInLevelView )
		{
			g_BackupCamera = Pine::RenderManager->GetRenderingContext( )->m_Camera;

			Pine::RenderManager->GetRenderingContext( )->m_Camera = g_EditorCamera;

			// Prepare the stencil buffer for the outline for the selected entity.
			if ( !Editor::Gui::Globals::SelectedEntityPtrs.empty( ) )
			{
				const auto entity = Editor::Gui::Globals::SelectedEntityPtrs[ 0 ];
				const auto modelRenderer = entity->GetComponent<Pine::ModelRenderer>( );

				if ( modelRenderer )
				{
					modelRenderer->OverrideStencilBuffer( true, 0xFF );
				}
			}
		}
	}

	void OnPostEntityRender( )
	{
		RenderHighlightedEntity( );
	}

	void OnPostRenderFinal( )
	{
		if ( Editor::Gui::Globals::IsInLevelView )
		{
			Pine::RenderManager->GetRenderingContext( )->m_Camera = g_BackupCamera;
		}
	}

	void OnRenderingPass( int pass )
	{
		if ( pass == 0 )
			OnPreRender( );
		if ( pass == 1 )
			OnPostEntityRender( );
		if ( pass == 2 )
			OnPostRenderFinal( );
	}
}

void Editor::RenderingHandler::Setup( )
{
	g_RenderingFrameBuffer = new Pine::FrameBuffer( );
	g_RenderingFrameBuffer->Create( 1600, 900 );

	Pine::RenderManager->GetRenderingContext( )->m_FrameBuffer = g_RenderingFrameBuffer;

	Pine::RenderManager->GetRenderingContext( )->m_Width = 1920;
	Pine::RenderManager->GetRenderingContext( )->m_Height = 1080;

	Pine::RenderManager->GetRenderingContext( )->m_AutoUpdateSize = false;

	Pine::RenderManager->SetRenderingCallback( OnRenderingPass );

	g_EditorCamera = Editor::EditorEntity::GetEntity( )->GetComponent<Pine::Camera>( );
	g_OutlineShader = Pine::Assets->GetAsset<Pine::Shader>( "Assets\\Editor\\Shaders\\ObjectOutline.shr" );
}

Pine::FrameBuffer* Editor::RenderingHandler::GetFrameBuffer( )
{
	return g_RenderingFrameBuffer;
}

void Editor::RenderingHandler::SetViewportSize( const int w, const int h )
{
	Pine::RenderManager->GetRenderingContext( )->m_Width = w;
	Pine::RenderManager->GetRenderingContext( )->m_Height = h;
}
