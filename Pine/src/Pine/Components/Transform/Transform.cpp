#include "Transform.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/fwd.hpp>

#include "../../../ImGui/imgui.h"
#include "../../Core/Serialization/Serialization.hpp"

#include "../../Entity/Entity.hpp"

void Pine::Transform::BuildDirections( )
{
    const auto rotation = GetRotationSum( );

    m_Forward = rotation * glm::vec3(0.f, 0.f, -1.0f);
    m_Up = rotation * glm::vec3(0.f, 1.f, 0.0f);
    m_Right = rotation * glm::vec3(1.f, 0.f, 0.0f);
}

void Pine::Transform::BuildTransformationMatrix( )
{
	m_TransformationMatrix = glm::mat4( 1.f );

	glm::vec3 pos( 0.f );

	if ( GetParent( )->GetParent( ) != nullptr )
	{
		pos = GetParent( )->GetParent( )->GetTransform( )->Position;
	}

	pos += Position;

	m_TransformationMatrix = glm::translate( m_TransformationMatrix, pos );

    m_TransformationMatrix *= glm::toMat4(Rotation);

	m_TransformationMatrix = glm::scale( m_TransformationMatrix, Scale );
}

Pine::Transform::Transform( )
{
	m_ComponentType = ComponentType::Transform;
    m_EulerAngles = glm::vec3( 0.f );

    Rotation = glm::quat( m_EulerAngles );
}

void Pine::Transform::OnSetup( )
{
}

void Pine::Transform::OnUpdate( float deltaTime )
{
}

void Pine::Transform::OnRender( )
{
    // I don't like this at all.

    bool update = false;

	if ( Position != m_LastPosition )
	{
		update = true;
	}
	else if ( true )
	{
		update = true;
	}
	else if ( Scale != m_LastScale )
	{
		update = true;
	}

	if ( GetParent( ) && GetParent( )->GetParent( ) != nullptr )
		update = true;

	if ( !update )
	{
		return;
	}

    m_LastPosition = Position;
	m_LastRotation = Rotation;
	m_LastScale = Scale;

    m_EulerAngles = glm::degrees(glm::eulerAngles(Rotation));
    EulerAngles = static_cast<glm::vec3>(m_EulerAngles);

	BuildTransformationMatrix( );
	BuildDirections( );
}

void Pine::Transform::SaveToJson( nlohmann::json& j )
{
	Serialization::SaveVec3( j[ "pos" ], Position );
	Serialization::SaveVec3( j[ "rot" ], glm::degrees( glm::eulerAngles( Rotation ) ) );
	Serialization::SaveVec3( j[ "scl" ], Scale );
}

void Pine::Transform::LoadFromJson( nlohmann::json& j )
{
	Position = Serialization::LoadVec3( j, "pos" );
	Scale = Serialization::LoadVec3( j, "scl" );

    SetEulerAngles(Serialization::LoadVec3( j, "rot" ));
}

glm::mat4& Pine::Transform::GetTransformationMatrix( )
{
	return m_TransformationMatrix;
}

const glm::vec3& Pine::Transform::GetForward( ) const
{
	return m_Forward;
}

const glm::vec3& Pine::Transform::GetRight( ) const
{
	return m_Right;
}

const glm::vec3& Pine::Transform::GetUp( ) const
{
	return m_Up;
}

glm::vec3 Pine::Transform::GetPositionSum() const
{
    glm::vec3 pos = Position;

    if (m_Parent->GetParent() != nullptr)
    {
        pos += m_Parent->GetParent()->GetTransform()->GetPositionSum();
    }

    return pos;
}

glm::quat Pine::Transform::GetRotationSum( ) const
{
    glm::quat rot = Rotation;

    if (m_Parent->GetParent() != nullptr)
    {
        rot *= m_Parent->GetParent()->GetTransform()->GetRotationSum();
    }

    return glm::normalize( rot );
}

void Pine::Transform::SetEulerAngles(glm::vec3 eulerAngles)
{
   Rotation = glm::quat(glm::radians(eulerAngles));
}

void Pine::Transform::Rotate(const glm::vec3 &eulerAngles) {
    SetEulerAngles(m_EulerAngles + eulerAngles);
}
