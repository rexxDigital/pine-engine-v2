#include "MainMenuBar.hpp"

#include "../Windows/Windows.hpp"
#include "../../ProjectManager/ProjectManager.hpp"

#include <imgui/imgui.h>

void Editor::Gui::MainMenuBar::Render() {
		if (ImGui::BeginMainMenuBar()) {

		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Save"))
			{
				ProjectManager::Save();
			}

			if (ImGui::MenuItem("Level Settings"))
			{

			}

			ImGui::Separator();

			if (ImGui::MenuItem("Exit"))
			{

			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Assets")) {

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Windows")) {

			ImGui::MenuItem("Asset Browser", nullptr, &Windows::ShowAssetBrowser);
			ImGui::MenuItem("Entity List", nullptr, &Windows::ShowEntitylist);
			ImGui::MenuItem("Properties", nullptr, &Windows::ShowProperties);
			ImGui::MenuItem("Game Viewport", nullptr, &Windows::ShowGameViewport);
			ImGui::MenuItem("Level Viewport", nullptr, &Windows::ShowLevelViewport);

			if (ImGui::BeginMenu("Debug")) {
				ImGui::MenuItem("Rendering Context", nullptr, &Windows::ShowRenderingContext);

				ImGui::EndMenu();
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	
}
