#include "Transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "../../../ImGui/imgui.h"
#include "../../Gui/Widgets/Common/CommonWidgets.hpp"

void Pine::Transform::BuildTransformationMatrix() {
	m_TransformationMatrix = glm::mat4(1.f);

	m_TransformationMatrix = glm::translate(m_TransformationMatrix, Position);

	m_TransformationMatrix = glm::rotate(m_TransformationMatrix, glm::radians(Rotation.x), glm::vec3(1.f, 0.f, 0.f));
	m_TransformationMatrix = glm::rotate(m_TransformationMatrix, glm::radians(Rotation.y), glm::vec3(0.f, 1.f, 0.f));
	m_TransformationMatrix = glm::rotate(m_TransformationMatrix, glm::radians(Rotation.z), glm::vec3(0.f, 0.f, 1.f));

	m_TransformationMatrix = glm::scale(m_TransformationMatrix, Scale);
}

Pine::Transform::Transform() {
	m_ComponentType = EComponentType::Transform;
}

void Pine::Transform::OnSetup() {
}

void Pine::Transform::OnUpdate(float deltaTime) {
}

void Pine::Transform::OnRender() {
	// FIXME: This is really ugly code...
	float posLength = Position.x + Position.y + Position.z;
	float rotLength = Rotation.x + Rotation.y + Rotation.z;
	float sclLength = Scale.x + Scale.y + Scale.z;

	bool update = false;

	if (posLength != m_LastPositionLen) {
		update = true;
	}
	else if (rotLength != m_LastRotationLen) {
		update = true;
	}
	else if (sclLength != m_LastScaleLen) {
		update = true;
	}

	if (!update) {
		return;
	}

	m_LastPositionLen = posLength;
	m_LastRotationLen = rotLength;
	m_LastScaleLen = sclLength;

	BuildTransformationMatrix();
}

void Pine::Transform::OnRenderGui()
{
	Gui::Widgets::Vector3("Position", Position);
	Gui::Widgets::Vector3("Rotation", Rotation);
	Gui::Widgets::Vector3("Scale", Scale);
}

const glm::mat4& Pine::Transform::GetTransformationMatrix() const
{
	return m_TransformationMatrix;
}
