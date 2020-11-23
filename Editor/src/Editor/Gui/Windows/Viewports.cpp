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
			Globals::IsInLevelView = false;
			
			ShowViewportControls(false);

			const auto avSize = ImGui::GetContentRegionAvail();
			
			ImGui::Image(reinterpret_cast<ImTextureID>(RenderingHandler::GetFrameBuffer()->GetTextureId()), avSize, ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));

		}

		ImGui::End( );
	}


	// --- Level viewport ---

	if ( ShowLevelViewport ) {
		if (ImGui::Begin( "Level", &ShowLevelViewport, ImGuiWindowFlags_MenuBar ))
		{
			Globals::IsInLevelView = true;
			
			ShowViewportControls(true);

			const auto avSize = ImGui::GetContentRegionAvail();

			RenderingHandler::SetViewportSize(avSize.x, avSize.y);

			ImVec2 cursorPos = ImGui::GetCursorScreenPos();  
			ImGuizmo::SetRect(cursorPos.x, cursorPos.y, avSize.x, avSize.y);
			
			ImGui::Image(reinterpret_cast<ImTextureID>(RenderingHandler::GetFrameBuffer()->GetTextureId()), avSize, ImVec2(0.f, 1.f), ImVec2(1.f, 0.f));

			Globals::IsHoveringLevelView = ImGui::IsItemHovered();
			
			if (!Editor::Gui::Globals::SelectedEntityPtrs.empty())
			{
				auto e = Globals::SelectedEntityPtrs[0];
				auto cam = EditorEntity::GetCamera();

				if (cam != nullptr)
				{
					ImGuizmo::OPERATION op;

					if (Globals::SelectedGizmoMovementType == GizmoMovementType::Move)
						op = ImGuizmo::OPERATION::TRANSLATE;
					if (Globals::SelectedGizmoMovementType == GizmoMovementType::Rotate)
						op = ImGuizmo::OPERATION::ROTATE;
					if (Globals::SelectedGizmoMovementType == GizmoMovementType::Scale)
						op = ImGuizmo::OPERATION::SCALE;

					if (ImGuizmo::Manipulate(glm::value_ptr(cam->GetViewMatrix()), glm::value_ptr(cam->GetProjectionMatrix()), op, ImGuizmo::WORLD, glm::value_ptr(e->GetTransform()->GetTransformationMatrix()), nullptr, nullptr))
					{
						float translation[3];
						float rotation[3];
						float scale[3];
						
						ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(e->GetTransform()->GetTransformationMatrix()), translation, rotation, scale);

						e->GetTransform()->Position = glm::vec3(translation[0], translation[1], translation[2]);
						e->GetTransform()->Rotation = glm::vec3(rotation[0], rotation[1], rotation[2]);
						e->GetTransform()->Scale = glm::vec3(scale[0], scale[1], scale[2]);
					}		
				}
			}
		}

		ImGui::End( );
	}

}