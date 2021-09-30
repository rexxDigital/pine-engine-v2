#include "RigidBody.hpp"

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../PhysicsManager/PhysicsManager.hpp"
#include "../../Entity/Entity.hpp"
#include "../../Core/Log/Log.hpp"

Pine::RigidBody::RigidBody( )
{
	m_ComponentType = EComponentType::RigidBody;
}

reactphysics3d::RigidBody* Pine::RigidBody::GetRigidBody( ) const
{
	return m_RigidBody;
}

void Pine::RigidBody::SetMass( float mass ) const
{
	m_RigidBody->setMass( mass );
}

float Pine::RigidBody::GetMass( ) const
{
	return m_RigidBody->getMass( );
}

void Pine::RigidBody::SetGravityEnabled( bool value ) const
{
	m_RigidBody->enableGravity( value );
}

bool Pine::RigidBody::GetGravityEnabled( ) const
{
	return m_RigidBody->isGravityEnabled( );
}

void Pine::RigidBody::SetRigidBodyType( RigidBodyType type )
{
	m_RigidBodyType = type;

	switch ( type )
	{
	case RigidBodyType::Static:
		m_RigidBody->setType( reactphysics3d::BodyType::STATIC );
		break;
	case RigidBodyType::Kinematic:
		m_RigidBody->setType( reactphysics3d::BodyType::KINEMATIC );
		break;
	case RigidBodyType::Dynamic:
		m_RigidBody->setType( reactphysics3d::BodyType::DYNAMIC );
		break;
	}
}

Pine::RigidBodyType Pine::RigidBody::GetRigidBodyType( ) const
{
	return m_RigidBodyType;
}

void Pine::RigidBody::OnPrePhysicsUpdate( ) const
{
	const auto transform = GetParent( )->GetTransform( );
	const auto rotQuat = glm::quat( transform->Rotation );

	reactphysics3d::Transform tr;

	const auto rotRadians = glm::radians( transform->Rotation );

	tr.setPosition( reactphysics3d::Vector3( transform->Position.x, transform->Position.y, transform->Position.z ) );
	tr.setOrientation( reactphysics3d::Quaternion::fromEulerAngles( reactphysics3d::Vector3( rotRadians.x, rotRadians.y, rotRadians.z ) ) );

	m_RigidBody->setTransform( tr );
}

void Pine::RigidBody::OnPostPhysicsUpdate( ) const
{
	const auto transform = GetParent( )->GetTransform( );
	const auto& physTransform = m_RigidBody->getTransform( );

	transform->Rotation = glm::eulerAngles( glm::quat( physTransform.getOrientation( ).x, physTransform.getOrientation( ).y, physTransform.getOrientation( ).z, physTransform.getOrientation( ).w ) );
	transform->Position = glm::vec3( physTransform.getPosition( ).x, physTransform.getPosition( ).y, physTransform.getPosition( ).z );
}

void Pine::RigidBody::OnCreated( )
{
	m_RigidBody = PhysicsManager::CreateRigidBody( m_PhysicsTransform );
}

void Pine::RigidBody::OnCopied( const IComponent* old )
{
	const auto oldRigidBody = dynamic_cast< const Pine::RigidBody* >( old );

	SetMass( oldRigidBody->GetMass( ) );
	SetGravityEnabled( oldRigidBody->GetGravityEnabled( ) );
	SetRigidBodyType( oldRigidBody->GetRigidBodyType( ) );
}

void Pine::RigidBody::OnDestroyed( )
{
	PhysicsManager::DestroyRigidBody( m_RigidBody );
}

void Pine::RigidBody::OnSetup( )
{
}

void Pine::RigidBody::OnUpdate( float deltaTime )
{

}

void Pine::RigidBody::SaveToJson( nlohmann::json& j )
{
	j[ "rgType" ] = m_RigidBodyType;
	j[ "mass" ] = m_RigidBody->getMass( );
	j[ "gvEnabled" ] = m_RigidBody->isGravityEnabled( );
} 

void Pine::RigidBody::LoadFromJson( nlohmann::json& j )
{
	SetRigidBodyType( static_cast< Pine::RigidBodyType >( j[ "rgType" ].get<int>( ) ) );
	m_RigidBody->setMass( m_RigidBody->getMass( ) );
	m_RigidBody->enableGravity( j[ "gvEnabled" ] );
}
