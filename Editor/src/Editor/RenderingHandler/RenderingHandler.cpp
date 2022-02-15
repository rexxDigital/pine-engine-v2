#include "RenderingHandler.hpp"

#include <glm/ext/matrix_transform.hpp>
#include <Pine/Pine.hpp>
#include <Pine/Rendering/RenderManager/RenderManager.hpp>
#include <Pine/OpenGL/FrameBuffer/FrameBuffer.hpp>

#include <ImGui/imgui.h>

#include "Editor/EditorEntity/EditorEntity.hpp"
#include "Editor/Gui/Gui.hpp"
#include "Editor/ProjectManager/ProjectManager.hpp"
#include "Pine/Components/Components.hpp"
#include "Pine/Components/Collider3D/Collider3D.hpp"
#include "Pine/Components/ModelRenderer/ModelRenderer.hpp"

void RenderPicPanelTexture( );

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

		if ( modelRenderer && modelRenderer->GetModel( ) )
		{
			Pine::Renderer3D->SetStencilFunction( 0x0205, 0xFF );
			Pine::Renderer3D->SetStencilMask( 0x00 );
			Pine::Renderer3D->SetDepthTesting( false );

			if ( const auto model = modelRenderer->GetModel( ) )
			{
				for ( const auto mesh : model->GetMeshList( ) )
				{
					Pine::Renderer3D->PrepareMesh( mesh );
					Pine::Renderer3D->SetShader( g_OutlineShader );

					g_OutlineShader->Use( );

					glm::mat4 mat = entity->GetTransform( )->GetTransformationMatrix( );

					// I did some testing around and found a nice size for the border.

					const auto camPosition = Editor::EditorEntity::GetEntity( )->GetTransform( )->Position;
					const auto entPosition = entity->GetTransform( )->Position;

					const float distance = glm::length( camPosition - entPosition );
					const float scaleSize = 1.02f + ( 0.0031105f * distance );

					mat = glm::scale( mat, glm::vec3( scaleSize, scaleSize, scaleSize ) );

					Pine::Renderer3D->RenderMesh( mat );
				}
			}

			Pine::Renderer3D->SetStencilMask( 0xFF );
			Pine::Renderer3D->SetStencilFunction( 0x0207, 0xFF );
			Pine::Renderer3D->SetDepthTesting( true );
		}
	}

	// Renders a preview of the collision boxes on the selected entity.
	void RenderSelectedColliderAABB( )
	{
		static Pine::Entity* dummyEntity = nullptr;
		static Pine::ModelRenderer* dummyModelRenderer = nullptr;

		if ( dummyEntity == nullptr )
		{
			dummyEntity = new Pine::Entity( 0 );
			dummyEntity->AddComponent( Pine::Components->CreateComponent( Pine::ComponentType::ModelRenderer, true ) );
			dummyModelRenderer = dummyEntity->GetComponent<Pine::ModelRenderer>( );
		}

		if ( Editor::Gui::Globals::SelectedEntityPtrs.empty( ) )
			return;

		const auto entity = Editor::Gui::Globals::SelectedEntityPtrs[ 0 ];
		const auto collider3D = entity->GetComponent<Pine::Collider3D>( );

		if ( collider3D )
		{
			Pine::Model* model;

			static auto boxPrimitiveModel = Pine::Assets->GetAsset<Pine::Model>( "Assets\\Engine\\Primitive Shapes\\cube.fbx" );
			static auto capsulePrimitiveModel = Pine::Assets->GetAsset<Pine::Model>( "Assets\\Engine\\Primitive Shapes\\capsule.fbx" );
			static auto spherePrimitiveModel = Pine::Assets->GetAsset<Pine::Model>( "Assets\\Engine\\Primitive Shapes\\sphere.fbx" );

			static auto boxColliderShader = Pine::Assets->GetAsset<Pine::Shader>( "Assets\\Editor\\Shaders\\ColliderBox.shr" );

			glm::vec3 scaleSize = collider3D->GetSize( );

			switch ( collider3D->GetColliderType( ) )
			{
			case Pine::ColliderType::Box:
				model = boxPrimitiveModel; break;
			case Pine::ColliderType::Capsule:
				model = capsulePrimitiveModel;
				scaleSize.x = collider3D->GetSize( ).x;
				scaleSize.y = collider3D->GetSize( ).y;
				scaleSize.z = 1.f;
				break;
			case Pine::ColliderType::Sphere:
				model = spherePrimitiveModel;
				scaleSize = glm::vec3( collider3D->GetSize( ).x );
				break;
			default:
				model = boxPrimitiveModel; break;
			}

			const auto mesh = model->GetMeshList( )[ 0 ];

			dummyEntity->GetTransform( )->Position = entity->GetTransform( )->Position + collider3D->GetPosition( );
			dummyEntity->GetTransform( )->Scale = scaleSize * entity->GetTransform( )->Scale;
			dummyEntity->GetTransform( )->Rotation = entity->GetTransform( )->Rotation;

			if ( collider3D->GetColliderType( ) == Pine::ColliderType::Capsule )
			{
				dummyEntity->GetTransform( )->Rotation.x += 90.f;
				dummyEntity->GetTransform( )->Rotation.y += 90.f;
			}

			if ( collider3D->GetColliderType( ) == Pine::ColliderType::Sphere )
			{
				dummyEntity->GetTransform( )->Rotation.x += 90.f;
			}

			dummyEntity->GetTransform( )->OnRender( );

			Pine::Renderer3D->PrepareMesh( mesh );
			Pine::Renderer3D->SetShader( boxColliderShader );

			Pine::Renderer3D->SetDepthTesting( false );
			Pine::Renderer3D->SetWireframeMode( true );

			boxColliderShader->Use( );

			Pine::Renderer3D->RenderMesh( dummyEntity->GetTransform( )->GetTransformationMatrix( ) );

			Pine::Renderer3D->SetDepthTesting( true );
			Pine::Renderer3D->SetWireframeMode( false );

		}
	}

	void OnPreRender( )
	{
		Editor::ProjectManager::Update( );

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
		if ( Editor::Gui::Globals::IsInLevelView )
		{
			//RenderHighlightedEntity( );
			RenderSelectedColliderAABB( );
		}
	}

	void OnPostRenderFinal( )
	{
		if ( Editor::Gui::Globals::IsInLevelView )
		{
			Pine::RenderManager->GetRenderingContext( )->m_Camera = g_BackupCamera;
		}

		RenderPicPanelTexture(  );
	}

	void OnRenderingPass( Pine::RenderStage stage )
	{
		if ( stage == Pine::RenderStage::PreRender )
			OnPreRender( );
		if ( stage == Pine::RenderStage::PostRenderEntities )
			OnPostEntityRender( );
		if ( stage == Pine::RenderStage::PostRender )
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
