#include "Transform.hpp"
#include <glm/gtc/matrix_transform.hpp>

#include "../../../ImGui/imgui.h"
#include "../../Core/Serialization/Serialization.hpp"

#include "../../Entity/Entity.hpp"

void Pine::Transform::BuildDirections( )
{
	m_Forward = glm::vec3(
		cos( glm::radians( Rotation.x ) ) * sin( glm::radians( Rotation.y ) ),
		sin( glm::radians( Rotation.x ) ),
		cos( glm::radians( Rotation.x ) ) * cos( glm::radians( Rotation.y ) )
	);

	m_Right = glm::vec3(
		sin( glm::radians( Rotation.y ) - 3.14f / 2.0f ),
		0,
		cos( glm::radians( Rotation.y ) - 3.14f / 2.0f )
	);

	m_Up = glm::cross( m_Right, m_Forward );
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

	m_TransformationMatrix = glm::rotate( m_TransformationMatrix, glm::radians( Rotation.x ), glm::vec3( 1.f, 0.f, 0.f ) );
	m_TransformationMatrix = glm::rotate( m_TransformationMatrix, glm::radians( Rotation.y ), glm::vec3( 0.f, 1.f, 0.f ) );
	m_TransformationMatrix = glm::rotate( m_TransformationMatrix, glm::radians( Rotation.z ), glm::vec3( 0.f, 0.f, 1.f ) );

	m_TransformationMatrix = glm::scale( m_TransformationMatrix, Scale );
}

Pine::Transform::Transform( )
{
	m_ComponentType = ComponentType::Transform;
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
	else if ( Rotation != m_LastRotation )
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

	BuildTransformationMatrix( );
	BuildDirections( );
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
	Scale = Serialization::LoadVec3( j, "scl" );
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

glm::vec3 Pine::Transform::GetRotationSum( ) const
{
    glm::vec3 rot = Rotation;

    if (m_Parent->GetParent() != nullptr)
    {
        rot += m_Parent->GetParent()->GetTransform()->GetRotationSum();
    }

    return rot;
}
