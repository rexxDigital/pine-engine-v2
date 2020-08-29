#include "CommonWidgets.hpp"

#include "../../../../ImGui/imgui.h"

void Gui::Widgets::Vector3(const std::string& str, glm::vec3& vec)
{
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
	const std::string& path = currentAsset->GetPath().string();

	ImGui::Text("%s (%s)", str.c_str(), Pine::SAssetType[static_cast<int>(type)]);
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
