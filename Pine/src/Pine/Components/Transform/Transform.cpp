#include "Transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "../../../ImGui/imgui.h"
#include "../../Core/Serialization/Serialization.hpp"

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

Pine::IComponent* Pine::Transform::Clone( )
{
	auto transform = new Pine::Transform( );

	transform->Position = Position;
	transform->Rotation = Rotation;
	transform->Scale = Scale;
	
	return transform;
}

void Pine::Transform::SaveToJson( nlohmann::json& j )
{
	Serialization::SaveVec3( j[ "pos" ], Position );
	Serialization::SaveVec3( j[ "rot" ], Rotation );
	Serialization::SaveVec3( j[ "scl" ], Scale );
}

void Pine::Transform::LoadFromJson( nlohmann::json& j )
{
	Position = Serialization::LoadVec3( j, "pos" );
	Rotation = Serialization::LoadVec3( j, "rot" );
	Scale	 = Serialization::LoadVec3( j, "scl" );
}

const glm::mat4& Pine::Transform::GetTransformationMatrix() const
{
	return m_TransformationMatrix;
}
