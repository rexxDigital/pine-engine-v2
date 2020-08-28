#include "Viewport.hpp"
#include "ImGui/imgui.h"
#include "../../Gui.hpp"

void Editor::Gui::Viewport::Run() {
	
	ImGui::Begin("Level View", nullptr, 0);
	{
		ImGui::Image(reinterpret_cast<ImTextureID>(Editor::Gui::GetViewportFrameBuffer()->GetTextureId()), ImVec2(700.f, 370.f));
	}
	ImGui::End();
	
	ImGui::Begin("Game View", nullptr, 0);
	{
	}
	ImGui::End();

}
