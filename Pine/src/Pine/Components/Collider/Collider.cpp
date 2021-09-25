#include "Collider.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../Entity/Entity.hpp"
#include "../../PhysicsManager/PhysicsManager.hpp"

Pine::Collider::Collider( )
{
	m_ComponentType = EComponentType::Collider;
}

void Pine::Collider::SetColliderType( ColliderType type )
{
	if ( m_Shape )
	{

	}

	m_Type = type;
}

Pine::ColliderType Pine::Collider::GetColliderType( ) const
{
	return m_Type;
}

void Pine::Collider::OnCreated( )
{
	m_RigidBody = PhysicsManager::GetPhysicsWorld( )->createRigidBody( m_PhysicsTransform );
}

void Pine::Collider::OnDestroyed( )
{
	PhysicsManager::GetPhysicsWorld( )->destroyRigidBody( m_RigidBody );
}

void Pine::Collider::OnSetup( )
{

}

void Pine::Collider::OnUpdate( float deltaTime )
{

}

void Pine::Collider::OnPrePhysicsUpdate( )
{
	auto transform = GetParent( )->GetTransform( );
	auto rotQuat = glm::quat( transform->Rotation );

	reactphysics3d::Transform tr;

	tr.setPosition( reactphysics3d::Vector3( transform->Position.x, transform->Position.y, transform->Position.z ) );
	tr.setOrientation( reactphysics3d::Quaternion( rotQuat.x, rotQuat.y, rotQuat.z, rotQuat.w ) );

	m_RigidBody->setTransform( tr );
}

void Pine::Collider::OnPostPhysicsUpdate( ) const
{
	const auto transform = GetParent( )->GetTransform( );
	const auto physTransform = m_RigidBody->getTransform( );

	transform->Rotation = glm::eulerAngles( glm::quat( physTransform.getOrientation( ).x, physTransform.getOrientation( ).y, physTransform.getOrientation( ).z, physTransform.getOrientation( ).w ) );
	transform->Position = glm::vec3( physTransform.getPosition( ).x, physTransform.getPosition( ).y, physTransform.getPosition( ).z );
}

void Pine::Collider::SaveToJson( nlohmann::json& j )
{
}

void Pine::Collider::LoadFromJson( nlohmann::json& j )
{

}
