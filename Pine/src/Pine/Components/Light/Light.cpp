#include "Light.hpp"

Pine::Light::Light() {
	m_ComponentType = EComponentType::Light;
}

void Pine::Light::SetLightColor(glm::vec3 lightColor) {
	m_LightColor = lightColor;
}

const glm::vec3& Pine::Light::GetLightColor() const {
	return m_LightColor;
}

void Pine::Light::OnSetup() {}

void Pine::Light::OnUpdate(float deltaTime) {}
