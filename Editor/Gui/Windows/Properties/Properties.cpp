#include "Properties.hpp"
#include "ImGui/imgui.h"
#include "../../Gui.hpp"
#include <Pine/Entity/Entity.hpp>

namespace {
	
	void DisplayEntity(Pine::Entity* e) {

		bool active = e->GetActive();
		if (ImGui::Checkbox("Active", &active)) {
			e->SetActive(active);
		}

		char buff[64];
		strcpy_s(buff, e->GetName().c_str());
		if (ImGui::InputText("##EntityName", buff, 64)) {
			e->SetName(buff);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		for (auto component : e->GetComponents())
		{
			if (ImGui::CollapsingHeader(Pine::SComponentNames[static_cast<int>(component->GetType())], ImGuiTreeNodeFlags_DefaultOpen))
			{
				// Show active check box
				bool active = component->GetActive();
				ImGui::Checkbox(std::string("Active##" + std::to_string(e->GetId()) + std::to_string(static_cast<int>(component->GetType()))).c_str(), &active);
				component->SetActive(active);

				// Using the engine's way to display the UI items.
				component->OnRenderGui();
			}

			ImGui::Spacing();
		}
	}

}

void Editor::Gui::Properties::Run() {
	
	const auto selectedEntity = Editor::Gui::GetSelectedEntity();

	ImGui::Begin("Properties", nullptr, 0);
	{
		if (selectedEntity != nullptr) {
			DisplayEntity(selectedEntity);
		}
		else {
			ImGui::Text("Please select an entity or an asset to preview\nit's properties here.");
		}

	}
	ImGui::End();

}
