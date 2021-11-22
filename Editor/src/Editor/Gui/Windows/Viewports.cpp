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
#include "Pine/Core/Math/Math.hpp"
#include "Pine/Rendering/DebugOverlay/DebugOverlay.hpp"

namespace {

	bool g_StartedPlaying = false;

	void ShowViewportControls( const bool inLevelViewport )
	{
		using namespace Editor::Gui;

		if ( ImGui::BeginMenuBar( ) ) 
		{
			if ( ImGui::MenuItem( "Transform", nullptr, Globals::SelectedGizmoMovementType == GizmoMovementType::Move, inLevelViewport ) ) 
			{
				Globals::SelectedGizmoMovementType = GizmoMovementType::Move;
			}

			if ( ImGui::MenuItem( "Rotate", nullptr, Globals::SelectedGizmoMovementType == GizmoMovementType::Rotate, inLevelViewport ) )
			{
				Globals::SelectedGizmoMovementType = GizmoMovementType::Rotate;
			}

			if ( ImGui::MenuItem( "Scale", nullptr, Globals::SelectedGizmoMovementType == GizmoMovementType::Scale, inLevelViewport ) ) 
			{
				Globals::SelectedGizmoMovementType = GizmoMovementType::Scale;
			}

			if ( inLevelViewport && Globals::IsHoveringLevelView )
			{
				if ( HotkeyManager::GetHotkeyPressed( Editor::Hotkeys::TransformGizmo ) )
				{
					Globals::SelectedGizmoMovementType = GizmoMovementType::Move;
				}

				if ( HotkeyManager::GetHotkeyPressed( Editor::Hotkeys::RotateGizmo ) )
				{
					Globals::SelectedGizmoMovementType = GizmoMovementType::Rotate;
				}

				if ( HotkeyManager::GetHotkeyPressed( Editor::Hotkeys::ScaleGizmo ) )
				{
					Globals::SelectedGizmoMovementType = GizmoMovementType::Scale;
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
					Globals::SelectedEntityPtrs.clear( );
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
		if ( ImGui::BeginDragDropTarget( ) )
		{
			if ( const auto payload = ImGui::AcceptDragDropPayload( "Asset", 0 ) ) 
			{
				const auto asset = *static_cast< Pine::IAsset** >( payload->Data );

				if ( asset->GetType( ) == Pine::EAssetType::Level )
				{
					if ( const auto level = dynamic_cast< Pine::Level* >( asset ) )
					{
						level->Load( );

						Editor::ProjectManager::OpenLevel( level );
					}
				}
				else if ( asset->GetType( ) == Pine::EAssetType::Model )
				{

				}
			}

			ImGui::EndDragDropTarget( );
		}
	}

	void RenderEntityIcon( const Pine::Entity* entity, Pine::Camera* camera, ImVec2 screenPosition )
	{
		constexpr float IconSize = 24.f;

		static auto cameraIcon = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\camera.png" );
		static auto lightIcon = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\light-bulb.png" );

		// Find the correct icon

		Pine::Texture2D* renderIcon = nullptr;

		for ( const auto component : entity->GetComponents( ) )
		{
			switch ( component->GetType( ) )
			{
				case Pine::EComponentType::Camera:
					renderIcon = cameraIcon;
					break;
				case Pine::EComponentType::Light:
					renderIcon = lightIcon;
					break;
				default:
					break;
			}
		}

		if ( !renderIcon )
			return;

		// Calculate the screen coordinates
		auto res = Pine::Math->WorldToScreen( entity->GetTransform( )->Position, camera );

		if ( res.z > 1.f ) // z > 1.f == Out of bounds 
			return;

		// To keep the icon centered
		res -= IconSize * 0.5f;

		ImGui::GetWindowDrawList( )->AddImage( reinterpret_cast< ImTextureID >( renderIcon->GetId( ) ), ImVec2( screenPosition.x + res.x, screenPosition.y + res.y ), ImVec2( screenPosition.x + res.x + IconSize, screenPosition.y + res.y + IconSize ) );
	}

}

void Editor::Gui::Windows::RenderViewports( )
{
	// --- Game viewport ---

	if ( ShowGameViewport ) 
	{
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

			Pine::DebugOverlay->SetViewport( cursorScreen.x, cursorScreen.y, avSize.x, avSize.y );
			Pine::DebugOverlay->Render( );
		}

		ImGui::End( );
	}


	// --- Level viewport ---

	if ( ShowLevelViewport ) 
	{
		if ( ImGui::Begin( "Level", &ShowLevelViewport, ImGuiWindowFlags_MenuBar ) )
		{
			Globals::IsInLevelView = true;

			ShowViewportControls( true );

			const auto avSize = ImGui::GetContentRegionAvail( );

			RenderingHandler::SetViewportSize( avSize.x, avSize.y );

			const ImVec2 cursorPos = ImGui::GetCursorScreenPos( );

			ImGuizmo::SetRect( cursorPos.x, cursorPos.y, avSize.x, avSize.y );

			ImGui::Image( reinterpret_cast< ImTextureID >( RenderingHandler::GetFrameBuffer( )->GetTextureId( ) ), avSize, ImVec2( 0.f, 0.f ), ImVec2( 1.f, 1.f ) );

			Pine::DebugOverlay->SetViewport( cursorPos.x, cursorPos.y, avSize.x, avSize.y );
			Pine::DebugOverlay->Render( );

			Globals::IsHoveringLevelView = ImGui::IsItemHovered( );

			HandleAssetViewportDrop( );

			const auto cam = EditorEntity::GetCamera( );

			// Render ImGuizmo

			if ( !Globals::SelectedEntityPtrs.empty( ) )
			{
				const auto e = Globals::SelectedEntityPtrs[ 0 ];

				if ( cam != nullptr )
				{
					ImGuizmo::OPERATION op;

					if ( Globals::SelectedGizmoMovementType == GizmoMovementType::Move )
						op = ImGuizmo::OPERATION::TRANSLATE;
					if ( Globals::SelectedGizmoMovementType == GizmoMovementType::Rotate )
						op = ImGuizmo::OPERATION::ROTATE;
					if ( Globals::SelectedGizmoMovementType == GizmoMovementType::Scale )
						op = ImGuizmo::OPERATION::SCALE;

					ImGuizmo::SetRect( cursorPos.x, cursorPos.y, avSize.x, avSize.y );
					ImGuizmo::SetDrawlist( ImGui::GetWindowDrawList( ) );

					// Because ImGuizmo has problems respecting the max view port width and height for whatever reason.
					ImGui::GetWindowDrawList( )->PushClipRect( ImVec2( cursorPos.x, cursorPos.y ), ImVec2( cursorPos.x + avSize.x, cursorPos.y + avSize.y ) );

					if ( Manipulate( glm::value_ptr( cam->GetViewMatrix( ) ), glm::value_ptr( cam->GetProjectionMatrix( ) ), op, ImGuizmo::WORLD, glm::value_ptr( e->GetTransform( )->GetTransformationMatrix( ) ), nullptr, nullptr ) )
					{
						float translation[ 3 ];
						float rotation[ 3 ];
						float scale[ 3 ];

						ImGuizmo::DecomposeMatrixToComponents( glm::value_ptr( e->GetTransform( )->GetTransformationMatrix( ) ), translation, rotation, scale );

						auto base_position = glm::vec3( 0.f );

						if ( e->GetParent( ) != nullptr )
							base_position = e->GetParent( )->GetTransform( )->Position;

						e->GetTransform( )->Position = glm::vec3( translation[ 0 ], translation[ 1 ], translation[ 2 ] ) - base_position;
						e->GetTransform( )->Rotation = ( glm::vec3( rotation[ 0 ], rotation[ 1 ], rotation[ 2 ] ) );
						e->GetTransform( )->Scale = glm::vec3( scale[ 0 ], scale[ 1 ], scale[ 2 ] );
					}

					ImGui::GetWindowDrawList( )->PopClipRect( );
				}
			}

			ImGui::GetWindowDrawList( )->PushClipRect( ImVec2( cursorPos.x, cursorPos.y ), ImVec2( cursorPos.x + avSize.x, cursorPos.y + avSize.y ) );

			for ( int i = 0; i < Pine::EntityList->GetEntities( ).size( ); i++ ) 
			{
				const auto entity = Pine::EntityList->GetEntity( i );

				if ( !entity->GetActive( ) )
					continue;
				if ( entity == EditorEntity::GetEntity( ) )
					continue;
				if ( !Globals::SelectedEntityPtrs.empty( ) && Globals::SelectedEntityPtrs[ 0 ] == entity )
					continue;

				RenderEntityIcon( entity, cam, cursorPos );
			}

			ImGui::GetWindowDrawList( )->PopClipRect( );
		}

		ImGui::End( );
	}

}