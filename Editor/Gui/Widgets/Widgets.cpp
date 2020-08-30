#include "Widgets.hpp"

#include <ImGui/imgui.h>
#include <Pine/Assets/Assets.hpp>

void Gui::Widgets::Vector3(const std::string& str, glm::vec3& vec) {
	ImGui::Text(str.c_str());

	ImGui::Columns(3, nullptr, false);

	ImGui::SetNextItemWidth(80.f);
	ImGui::DragFloat(std::string("X##" + str).c_str(), &vec.x, 0.1f, -FLT_MAX, FLT_MAX, "%.1f");
	ImGui::NextColumn();

	ImGui::SetNextItemWidth(80.f);
	ImGui::DragFloat(std::string("Y##" + str).c_str(), &vec.y, 0.1f, -FLT_MAX, FLT_MAX, "%.1f");
	ImGui::NextColumn();

	ImGui::SetNextItemWidth(80.f);
	ImGui::DragFloat(std::string("Z##" + str).c_str(), &vec.z, 0.1f, -FLT_MAX, FLT_MAX, "%.1f");

	ImGui::Columns(1);
}

Pine::IAsset* Gui::Widgets::AssetPicker(Pine::IAsset* currentAsset, const std::string& str, Pine::EAssetType type) {
	static auto unknownImage = Pine::Assets::GetAsset<Pine::Texture2D>("Engine\\Icons\\030-corrupt file.png");

	const std::string& path = currentAsset->GetPath().string();

	ImGui::Text("%s (%s)", str.c_str(), Pine::SAssetType[static_cast<int>(type)]);

	int image = unknownImage->GetId();
	if (currentAsset->HasAvailablePreview()) {
		image = currentAsset->GetAssetPreview();
	}

	ImGui::Image(reinterpret_cast<ImTextureID>(image), ImVec2(24, 24), ImVec2(1.f, 1.f), ImVec2(0.f, 0.f));

	if (!currentAsset->HasAvailablePreview() && ImGui::IsItemHovered()) {
		ImGui::SetTooltip("No preview available.");
	}

	ImGui::SameLine();

	ImGui::InputText(std::string("##AssetPicker" + str).c_str(), (char*)path.data(), path.size(), ImGuiInputTextFlags_ReadOnly);

	if (ImGui::BeginDragDropTarget()) {
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Asset")) {

		}

		ImGui::EndDragDropTarget();
	}

	ImGui::SameLine();

	if (ImGui::Button("...")) {

	}

	return currentAsset;
}

Pine::IAsset* Gui::Widgets::AssetBrowser(Pine::IAsset* selectedAsset, Pine::EAssetType filter /*= Pine::EAssetType::Invalid*/) {
	// Cached icons
	static auto folderIcon = Pine::Assets::GetAsset<Pine::Texture2D>("Engine\\Icons\\006-folder.png");
	static auto unknownFileIcon = Pine::Assets::GetAsset<Pine::Texture2D>("Engine\\Icons\\030-corrupt file.png");

	static auto currentPath = Pine::Assets::GetAssetsDirectoryCache();
	auto root = Pine::Assets::GetAssetsDirectoryCache();

	const bool inRootDirectory = root == currentPath;

	ImGui::BeginChild("##AssetBrowserArea", ImVec2(-1.f, -1.f), false);

	ImGui::Columns(11, nullptr, false);

	for (auto& item : currentPath->items) {
		auto asset = item->assetPointer;

		int icon = unknownFileIcon->GetId();
		std::string name = item->name;

		if (item->isDirectory) {
			icon = folderIcon->GetId();
		}
		else {
			if (asset && asset->HasAvailablePreview()) {
				icon = asset->GetAssetPreview();
			}
		}

		ImGui::PushID(item->name.c_str());
		ImGui::BeginGroup();

		if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(icon), ImVec2(48, 48), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f), 3)) {
			if (item->isDirectory) {
				if (item->name == "...") {
					currentPath = item->parent;
				}
				else {
					currentPath = item.get();
				}

				ImGui::EndGroup();
				ImGui::PopID();

				break;
			}
		}

		ImGui::Text(item->name.c_str());

		ImGui::EndGroup();
		ImGui::PopID();

		ImGui::NextColumn();
	}

	ImGui::Columns(1);

	ImGui::EndChild();

	return nullptr;
}
