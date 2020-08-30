#include "AssetBrowser.hpp"
#include "ImGui/imgui.h"
#include <Pine/Assets/Model/Model.hpp>
#include <Pine/Gui/Widgets/Common/CommonWidgets.hpp>

void Editor::Gui::AssetBrowser::Run() {
	

	ImGui::Begin("Asset Browser", nullptr, 0);
	{
		Pine::Gui::Widgets::AssetBrowser(nullptr, Pine::EAssetType::Invalid);
	}
	ImGui::End();

}
