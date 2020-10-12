#include "Windows.hpp"
#include "../Gui.hpp"

namespace {

	void ShowViewportControls( bool inLevelViewport ) {
		if ( ImGui::BeginMenuBar( ) ) {
			if ( ImGui::MenuItem( "Move", nullptr, Editor::Gui::Globals::SelectedGizmoMovementType == Editor::Gui::GizmoMovementType::Move, inLevelViewport ) ) {
				Editor::Gui::Globals::SelectedGizmoMovementType = Editor::Gui::GizmoMovementType::Move;
			}

			if ( ImGui::MenuItem( "Rotate", nullptr, Editor::Gui::Globals::SelectedGizmoMovementType == Editor::Gui::GizmoMovementType::Rotate, inLevelViewport ) ) {
				Editor::Gui::Globals::SelectedGizmoMovementType = Editor::Gui::GizmoMovementType::Rotate;
			}

			if ( ImGui::MenuItem( "Scale", nullptr, Editor::Gui::Globals::SelectedGizmoMovementType == Editor::Gui::GizmoMovementType::Scale, inLevelViewport ) ) {
				Editor::Gui::Globals::SelectedGizmoMovementType = Editor::Gui::GizmoMovementType::Scale;
			}

			ImGui::EndMenuBar( );
		}
	}

}

void Editor::Gui::Windows::RenderViewports( ) { 

	// --- Game viewport ---

	if ( ShowGameViewport ) {
		ImGui::Begin( "Game", &ShowGameViewport, ImGuiWindowFlags_MenuBar );

		ShowViewportControls( false );

		ImGui::End( );
	}


	// --- Level viewport ---

	if ( ShowLevelViewport ) {
		ImGui::Begin( "Level", &ShowLevelViewport, ImGuiWindowFlags_MenuBar );

		ShowViewportControls( true );

		ImGui::End( );
	}

}