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
