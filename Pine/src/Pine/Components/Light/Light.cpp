#include "Light.hpp"

#include "../../../ImGui/imgui.h"

Pine::Light::Light() {
	m_ComponentType = EComponentType::Light;
}

void Pine::Light::SetLightColor(glm::vec3 lightColor) {
	m_LightColor = lightColor;
}

const glm::vec3& Pine::Light::GetLightColor() const {
	return m_LightColor;
}

void Pine::Light::SetLightType(ELightType type) {
	m_LightType = type;
}

Pine::ELightType Pine::Light::GetLightType() const {
	return m_LightType;
}

void Pine::Light::OnSetup() {}

void Pine::Light::OnUpdate(float deltaTime) {}

void Pine::Light::OnRenderGui()
{
	ImGui::Text("Light Color");
	ImGui::ColorEdit3("##LightColor", reinterpret_cast<float*>(&m_LightColor));

	ImGui::Text("Light Type");
	ImGui::Combo("##LightType", reinterpret_cast<int*>(&m_LightType), "Directional\0Spot light");

}
