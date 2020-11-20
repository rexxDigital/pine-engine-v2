#include "Windows.hpp"
#include "../Gui.hpp"
#include "../../RenderingHandler/RenderingHandler.hpp"

#include <Pine/OpenGL/FrameBuffer/FrameBuffer.hpp>

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
		if (ImGui::Begin( "Game", &ShowGameViewport, ImGuiWindowFlags_MenuBar ))
		{
			ShowViewportControls(false);

			
			const auto avSize = ImGui::GetContentRegionAvail();

			RenderingHandler::SetViewportSize(static_cast<int>(avSize.x * (avSize.x / avSize.y)), static_cast<int>(avSize.y * (avSize.x / avSize.y)));
			ImGui::Image(reinterpret_cast<ImTextureID>(RenderingHandler::GetFrameBuffer()->GetTextureId()), avSize, ImVec2(1.f, 1.f), ImVec2(0.f, 0.f));
		}

		ImGui::End( );
	}


	// --- Level viewport ---

	if ( ShowLevelViewport ) {
		if (ImGui::Begin( "Level", &ShowLevelViewport, ImGuiWindowFlags_MenuBar ))
		{
			ShowViewportControls(true);

			const auto avSize = ImGui::GetContentRegionAvail();

			RenderingHandler::SetViewportSize(static_cast<int>(avSize.x * (avSize.x / avSize.y)), static_cast<int>(avSize.y * (avSize.x / avSize.y)));

			ImGui::Image(reinterpret_cast<ImTextureID>(RenderingHandler::GetFrameBuffer()->GetTextureId()), avSize, ImVec2(1.f, 1.f), ImVec2(0.f, 0.f));
		}

		ImGui::End( );
	}

}