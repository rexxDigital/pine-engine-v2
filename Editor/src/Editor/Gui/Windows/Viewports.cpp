#include <GLFW/glfw3.h>

#include "Windows.hpp"
#include "../Gui.hpp"
#include "../../RenderingHandler/RenderingHandler.hpp"

#include <Pine/OpenGL/FrameBuffer/FrameBuffer.hpp>

#include <ImGuizmo/ImGuizmo.h>
#include <glm/gtc/type_ptr.hpp>


#include "Editor/EditorEntity/EditorEntity.hpp"
#include "Pine/Rendering/RenderManager/RenderManager.hpp"

#include "Pine/Entity/Entity.hpp"
#include "Pine/Components/Transform/Transform.hpp"

#include "Editor/PlayManager/PlayManager.hpp"
#include "Pine/Entitylist/EntityList.hpp"

#include "Editor/Gui/Utility/HotkeyManager/HotkeyManager.hpp"
#include "Editor/ProjectManager/ProjectManager.hpp"
#include "Pine/Components/ModelRenderer/ModelRenderer.hpp"

namespace {

	bool g_StartedPlaying = false;

	void ShowViewportControls( bool inLevelViewport ) {
		using namespace Editor::Gui;

		if ( ImGui::BeginMenuBar( ) ) {
			if ( ImGui::MenuItem( "Transform", nullptr, Globals::SelectedGizmoMovementType == GizmoMovementType::Move, inLevelViewport ) ) {
				Editor::Gui::Globals::SelectedGizmoMovementType = Editor::Gui::GizmoMovementType::Move;
			}

			if ( ImGui::MenuItem( "Rotate", nullptr, Globals::SelectedGizmoMovementType == GizmoMovementType::Rotate, inLevelViewport ) ) {
				Editor::Gui::Globals::SelectedGizmoMovementType = Editor::Gui::GizmoMovementType::Rotate;
			}

			if ( ImGui::MenuItem( "Scale", nullptr, Globals::SelectedGizmoMovementType == GizmoMovementType::Scale, inLevelViewport ) ) {
				Editor::Gui::Globals::SelectedGizmoMovementType = GizmoMovementType::Scale;
			}

			if ( inLevelViewport && Globals::IsHoveringLevelView )
			{
				if ( Editor::Gui::HotkeyManager::GetHotkeyPressed( Editor::Hotkeys::TransformGizmo ) )
				{
					Editor::Gui::Globals::SelectedGizmoMovementType = Editor::Gui::GizmoMovementType::Move;
				}

				if ( Editor::Gui::HotkeyManager::GetHotkeyPressed( Editor::Hotkeys::RotateGizmo ) )
				{
					Editor::Gui::Globals::SelectedGizmoMovementType = Editor::Gui::GizmoMovementType::Rotate;
				}

				if ( Editor::Gui::HotkeyManager::GetHotkeyPressed( Editor::Hotkeys::ScaleGizmo ) )
				{
					Editor::Gui::Globals::SelectedGizmoMovementType = Editor::Gui::GizmoMovementType::Scale;
				}
			}

			ImGui::Spacing( );
			ImGui::Separator( );
			ImGui::Spacing( );

			const bool isPlaying = Editor::PlayManager::IsPlaying( );
			if ( ImGui::MenuItem( isPlaying ? "Stop" : "Start", nullptr ) )
			{
				if ( isPlaying )
				{
					Editor::PlayManager::Stop( );
					Editor::Gui::Globals::SelectedEntityPtrs.clear( );
				}
				else
				{
					Editor::PlayManager::Start( );

					g_StartedPlaying = true;
				}
			}

			ImGui::EndMenuBar( );
		}
	}

	void HandleAssetViewportDrop( )
	{
		if ( ImGui::BeginDragDropTarget( ) ) {
			if ( const auto payload = ImGui::AcceptDragDropPayload( "Asset", 0 ) ) {
				const auto asset = *static_cast< Pine::IAsset** >( payload->Data );

				if ( asset->GetType(  ) == Pine::EAssetType::Level )
				{
					if ( const auto level = dynamic_cast< Pine::Level* >( asset ) )
					{
						level->Load( );

						Editor::ProjectManager::OpenLevel( level );
					}
				}
				else if ( asset->GetType(  ) == Pine::EAssetType::Model )
				{
					
				}
			}

			ImGui::EndDragDropTarget( );
		}	
	}

	void RenderHighlightBoundingBox( Pine::ModelRenderer* modelRenderer, Pine::Camera* camera ) {

		auto model = modelRenderer->GetTargetModel( );

		if ( !model )
			return;

		for ( auto mesh : model->GetMeshList(  ) )
		{
			auto mins = mesh->GetMins( );
			auto maxs = mesh->GetMins( );

			
		}

	}

	void RenderEntityIcon( Pine::Entity* entity, Pine::Camera* camera, ImVec2 screenPosition, ImVec2 screenSize ) {
		int renderIcon = 0;
		Pine::IComponent* renderComponent = nullptr;

		for ( const auto component : entity->GetComponents( ) ) {
			switch ( component->GetType( ) ) {
			case Pine::EComponentType::Camera:
				renderIcon = 1;
				renderComponent = component;
				break;
			case Pine::EComponentType::Light:
				renderIcon = 2;
				renderComponent = component;
				break;
			default:
				break;
			}
		}

		if ( renderIcon == 0 ) {
			return;
		}
 
		auto transform = entity->GetTransform( );
		auto res = glm::project( transform->Position, camera->GetViewMatrix(  ), camera->GetProjectionMatrix( ) , glm::vec4( 0.f, 0.f, screenSize.x, screenSize.y ) );
		
		if ( renderIcon == 1 )
			ImGui::GetForegroundDrawList( )->AddText( ImVec2( screenPosition.x + res.x, screenPosition.y + res.y ), ImColor( 255, 255, 255, 255 ), "Camera" );
		else
			ImGui::GetForegroundDrawList( )->AddText( ImVec2( screenPosition.x + res.x, screenPosition.y + res.y ), ImColor( 255, 255, 255, 255 ), "Light" );
	}

	void RenderHighlight( Pine::Entity* e, Pine::Camera* camera, ImVec2 size ) {
		for ( const auto component : e->GetComponents(  ) ) {
			switch ( component->GetType(  ) ) {
			case Pine::EComponentType::ModelRenderer:
				RenderHighlightBoundingBox( dynamic_cast< Pine::ModelRenderer* >( component ), camera );
				return;
			default:
				break;
			}
		}
	}

}

void Editor::Gui::Windows::RenderViewports( ) {

	// --- Game viewport ---

	if ( ShowGameViewport ) {

		if ( g_StartedPlaying )
		{
			const auto& io = ImGui::GetIO( );

			if ( !io.KeyAlt )
				ImGui::SetNextWindowFocus( );

			g_StartedPlaying = false;
		}

		if ( ImGui::Begin( "Game", &ShowGameViewport, ImGuiWindowFlags_MenuBar ) )
		{
			Globals::IsInLevelView = false;

			ShowViewportControls( false );

			const auto avSize = ImGui::GetContentRegionAvail( );
			const auto cursorScreen = ImGui::GetCursorScreenPos( );

			if ( Pine::RenderManager->GetRenderingContext( )->m_Camera != nullptr )
			{
				ImGui::Image( reinterpret_cast< ImTextureID >( RenderingHandler::GetFrameBuffer( )->GetTextureId( ) ), avSize, ImVec2( 0.f, 0.f ), ImVec2( 1.f, 1.f ) );
			}
			else
			{
				ImGui::GetForegroundDrawList( )->AddText( ImVec2( cursorScreen.x + 10.f, cursorScreen.y + 10.f ), ImColor( 255, 255, 255, 255 ), "No active camera, please make at least one is active." );
			}
		}

		ImGui::End( );
	}


	// --- Level viewport ---

	if ( ShowLevelViewport ) {
		if ( ImGui::Begin( "Level", &ShowLevelViewport, ImGuiWindowFlags_MenuBar ) )
		{
			Globals::IsInLevelView = true;

			ShowViewportControls( true );

			const auto avSize = ImGui::GetContentRegionAvail( );

			RenderingHandler::SetViewportSize( avSize.x, avSize.y );

			const ImVec2 cursorPos = ImGui::GetCursorScreenPos( );

			ImGuizmo::SetRect( cursorPos.x, cursorPos.y, avSize.x, avSize.y );

			ImGui::Image( reinterpret_cast< ImTextureID >( RenderingHandler::GetFrameBuffer( )->GetTextureId( ) ), avSize, ImVec2( 0.f, 0.f ), ImVec2( 1.f, 1.f ) );

			Globals::IsHoveringLevelView = ImGui::IsItemHovered( );

			HandleAssetViewportDrop( );

			auto cam = EditorEntity::GetCamera( );

			if ( !Editor::Gui::Globals::SelectedEntityPtrs.empty( ) )
			{
				auto e = Globals::SelectedEntityPtrs[ 0 ];

				if ( cam != nullptr )
				{
					ImGuizmo::OPERATION op;

					if ( Globals::SelectedGizmoMovementType == GizmoMovementType::Move )
						op = ImGuizmo::OPERATION::TRANSLATE;
					if ( Globals::SelectedGizmoMovementType == GizmoMovementType::Rotate )
						op = ImGuizmo::OPERATION::ROTATE;
					if ( Globals::SelectedGizmoMovementType == GizmoMovementType::Scale )
						op = ImGuizmo::OPERATION::SCALE;

					if ( ImGuizmo::Manipulate( glm::value_ptr( cam->GetViewMatrix( ) ), glm::value_ptr( cam->GetProjectionMatrix( ) ), op, ImGuizmo::WORLD, glm::value_ptr( e->GetTransform( )->GetTransformationMatrix( ) ), nullptr, nullptr ) )
					{
						float translation[ 3 ];
						float rotation[ 3 ];
						float scale[ 3 ];

						ImGuizmo::DecomposeMatrixToComponents( glm::value_ptr( e->GetTransform( )->GetTransformationMatrix( ) ), translation, rotation, scale );

						glm::vec3 base_position = glm::vec3( 0.f );

						if ( e->GetParent( ) != nullptr )
							base_position = e->GetParent( )->GetTransform( )->Position;

						e->GetTransform( )->Position = glm::vec3( translation[ 0 ], translation[ 1 ], translation[ 2 ] ) - base_position;
						e->GetTransform( )->Rotation = ( glm::vec3( rotation[ 0 ], rotation[ 1 ], rotation[ 2 ] ) );
						e->GetTransform( )->Scale = glm::vec3( scale[ 0 ], scale[ 1 ], scale[ 2 ] );
					}

			//		RenderHighlight( e, cam, avSize );
				}
			}

			for ( const auto& entity : Pine::EntityList->GetEntities(  ) ) {
				if ( !entity.GetActive( ) )
					continue;

			//	RenderEntityIcon( entity, cam, cursorPos, avSize );
			}
		}

		ImGui::End( );
	}

}