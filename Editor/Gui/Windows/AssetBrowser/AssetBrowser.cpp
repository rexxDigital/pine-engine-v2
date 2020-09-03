#include "AssetBrowser.hpp"
#include "ImGui/imgui.h"
#include <Pine/Assets/Model/Model.hpp>
#include "../../Widgets/Widgets.hpp"

void Editor::Gui::AssetBrowser::Run() {
	
	
	ImGui::Begin("Asset Browser", nullptr, 0);
	{
		::Gui::Widgets::AssetBrowser(nullptr, Pine::EAssetType::Invalid);
	}
	ImGui::End();

}
