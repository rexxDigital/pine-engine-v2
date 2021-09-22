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

void Pine::Transform::BuildTransformationMatrix( ) {
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

Pine::Transform::Transform( ) {
	m_ComponentType = EComponentType::Transform;
}

void Pine::Transform::OnSetup( ) {
}

void Pine::Transform::OnUpdate( float deltaTime ) {
}

void Pine::Transform::OnRender( ) {
	// FIXME: This is really ugly code...
	const float posLength = Position.x + Position.y + Position.z;
	const float rotLength = Rotation.x + Rotation.y + Rotation.z;
	const float sclLength = Scale.x + Scale.y + Scale.z;

	bool update = false;

	if ( posLength != m_LastPositionLen ) {
		update = true;
	}
	else if ( rotLength != m_LastRotationLen ) {
		update = true;
	}
	else if ( sclLength != m_LastScaleLen ) {
		update = true;
	}

	if ( GetParent( )->GetParent( ) != nullptr )
		update = true;

	if ( !update ) {
		return;
	}

	m_LastPositionLen = posLength;
	m_LastRotationLen = rotLength;
	m_LastScaleLen = sclLength;

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
