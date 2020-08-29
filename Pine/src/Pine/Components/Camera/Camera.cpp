#include "Camera.hpp"
#include "../../Core/Window/Window.hpp"
#include "../../Entity/Entity.hpp"
#include "../Transform/Transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "../../../ImGui/imgui.h"
#include "../../Rendering/RenderManager/RenderManager.hpp"

void Pine::Camera::BuildProjectionMatrix() {
	const auto size = Window::GetSize();

	m_AspectRatio = static_cast<float>(size.x) / static_cast<float>(size.y);
	m_ProjectionMatrix = glm::perspective(glm::radians(m_FieldOfView), m_AspectRatio, m_NearPlane, m_FarPlane);
}

void Pine::Camera::BuildViewMatrix() {
	auto transform = m_Parent->GetTransform();

	const float verticalAngle = glm::radians(transform->Rotation.x);
	const float horizontalAngle = glm::radians(transform->Rotation.y);

	const glm::vec3 direction(
		std::cos(verticalAngle) * std::sin(horizontalAngle),
		std::sin(verticalAngle),
		std::cos(verticalAngle) * std::cos(horizontalAngle)
	);

	const glm::vec3 right = glm::vec3(
		std::sin(horizontalAngle - 3.14159265359f / 2.0f),
		0,
		std::cos(horizontalAngle - 3.14159265359f / 2.0f)
	);

	const glm::vec3 up = glm::cross(right, direction);

	m_ViewMatrix = glm::lookAt(transform->Position, transform->Position + direction, up);
}

Pine::Camera::Camera() {
	m_ComponentType = EComponentType::Camera;
}

float Pine::Camera::GetNearPlane() const {
	return m_NearPlane;
}

float Pine::Camera::GetFarPlane() const {
	return m_FarPlane;
}

float Pine::Camera::GetFieldOfView() const {
	return m_FieldOfView;
}

void Pine::Camera::SetNearPlane(float value) {
	m_NearPlane = value;
	BuildProjectionMatrix();
}

void Pine::Camera::SetFarPlane(float value) {
	m_FarPlane = value;
	BuildProjectionMatrix();
}

void Pine::Camera::SetFieldOfViewPlane(float value) {
	m_FieldOfView = value;
	BuildProjectionMatrix();
}

const glm::mat4& Pine::Camera::GetProjectionMatrix() const {
	return m_ProjectionMatrix;
}

const glm::mat4& Pine::Camera::GetViewMatrix() const {
	return m_ViewMatrix;
}

void Pine::Camera::OnSetup() {
	BuildProjectionMatrix();
}

void Pine::Camera::OnRender() {
	BuildViewMatrix();
}

void Pine::Camera::OnUpdate(float deltaTime) {

}

void Pine::Camera::OnRenderGui()
{
	ImGui::Text("Near Plane");
	
	if (ImGui::DragFloat("##NearPlane", &m_NearPlane, 0.01f, 0.001f, 1000.f))
	{
		BuildProjectionMatrix();
	}

	ImGui::Text("Far Plane");

	if (ImGui::DragFloat("##FarPlane", &m_FarPlane, 0.01f, 100.f, 1000.f))
	{
		BuildProjectionMatrix();
	}

	ImGui::Text("Field of View");
	
	if (ImGui::SliderFloat("##FoV", &m_FieldOfView, 30.f, 150.f))
	{
		BuildProjectionMatrix();
	}

	ImGui::Separator();

	if (RenderManager::GetCamera() == this)
		ImGui::Text("This camera is currently the active camera.");
	else
	{
		ImGui::Text("This camera is not currently the active camera.");

		if (ImGui::Button("Set active camera", ImVec2(-1.f, 30.f)))
		{
			Pine::RenderManager::SetCamera(this);
		}
	}
}
