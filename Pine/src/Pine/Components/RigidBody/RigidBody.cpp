#include "RigidBody.hpp"

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../PhysicsManager/PhysicsManager.hpp"
#include "../../Entity/Entity.hpp"
#include "../../Core/Log/Log.hpp"

#include "../Collider3D/Collider3D.hpp"

void Pine::RigidBody::UpdateColliders( )
{
	const auto collider = m_Parent->GetComponent<Pine::Collider3D>( );

	if ( !collider )
	{
		if ( m_Collider )
			m_RigidBody->removeCollider( m_Collider );

		m_Collider3D = nullptr;
		m_Collider = nullptr;

		return;
	}

	collider->OnPrePhysicsUpdate( );

	m_Collider3D = collider;

	if ( collider->PollShapeUpdated( ) )
	{
		if ( m_Collider )
			m_RigidBody->removeCollider( m_Collider );

		m_Collider = nullptr;
	}

	if ( !m_Collider )
		m_Collider = m_RigidBody->addCollider( collider->GetCollisionShape( ), m_ColliderTransform );

	if ( m_Collider && collider )
	{
		const auto transform = m_Parent->GetTransform( );

		reactphysics3d::Transform tr;

		tr.identity( );

        const auto rot = transform->GetRotationSum( );

		tr.setPosition( reactphysics3d::Vector3( collider->GetPosition( ).x, collider->GetPosition( ).y, collider->GetPosition( ).z ) );
		//tr.setOrientation( reactphysics3d::Quaternion( rot.x, rot.y, rot.z, rot.w ) );

		m_Collider->setLocalToBodyTransform( tr );
        m_Collider->getMaterial().setBounciness(0);
	}
}

Pine::RigidBody::RigidBody( )
{
	m_ComponentType = ComponentType::RigidBody;
}

reactphysics3d::RigidBody* Pine::RigidBody::GetRigidBody( ) const
{
	return m_RigidBody;
}

void Pine::RigidBody::SetMass( float mass )
{
	m_Mass = mass;
}

float Pine::RigidBody::GetMass( ) const
{
	return m_Mass;
}

void Pine::RigidBody::SetGravityEnabled( bool value )
{
	m_GravityEnabled = value;
}

bool Pine::RigidBody::GetGravityEnabled( ) const
{
	return m_GravityEnabled;
}

void Pine::RigidBody::SetRigidBodyType( RigidBodyType type )
{
	m_RigidBodyType = type;
}

Pine::RigidBodyType Pine::RigidBody::GetRigidBodyType( ) const
{
	return m_RigidBodyType;
}

void Pine::RigidBody::DetachCollider( )
{
	if ( m_Collider && m_RigidBody )
		m_RigidBody->removeCollider( m_Collider );

	m_Collider3D = nullptr;
	m_Collider = nullptr;
}

bool Pine::RigidBody::IsColliderAttached( Collider3D* collider ) const
{
	return m_Collider3D == collider;
}

void Pine::RigidBody::OnPrePhysicsUpdate( )
{
    if ( !m_RigidBody )
    {
        m_RigidBody = PhysicsManager->CreateRigidBody( m_RigidBodyTransform );
        m_RigidBody->setType( reactphysics3d::BodyType::DYNAMIC );

        UpdateRigidbodyProperties( );
    }

    if ( m_Parent->GetStatic( ) )
        return;

    UpdateRigidbodyProperties( );
}

void Pine::RigidBody::OnPostPhysicsUpdate( )
{
    if (m_Parent->GetStatic())
        return;
    if (!m_RigidBody)
        return;

	const auto transform = GetParent( )->GetTransform( );
	const auto& physTransform = m_RigidBody->getTransform( );

	transform->Rotation = glm::quat( physTransform.getOrientation( ).w, physTransform.getOrientation( ).x, physTransform.getOrientation( ).y, physTransform.getOrientation( ).z );
	transform->Position = glm::vec3( physTransform.getPosition( ).x, physTransform.getPosition( ).y, physTransform.getPosition( ).z );
}

void Pine::RigidBody::OnCreated( )
{
}

void Pine::RigidBody::OnCopied( const IComponent* old )
{
	m_RigidBody = nullptr;
}

void Pine::RigidBody::OnDestroyed( )
{
	//Log->Warning( "RigidBody::OnDestroyed( ) -> (" + m_Parent->GetName( ) + ")" );

	if ( m_RigidBody )
	{
		PhysicsManager->DestroyRigidBody( m_RigidBody );
	}

	m_RigidBody = nullptr;
}

void Pine::RigidBody::OnSetup( )
{
    if ( m_Standalone )
        return;
}

void Pine::RigidBody::OnUpdate( float deltaTime )
{
}

void Pine::RigidBody::SaveToJson( nlohmann::json& j )
{
	j[ "rgType" ] = m_RigidBodyType;
	j[ "mass" ] = m_Mass;
	j[ "gvEnabled" ] = m_GravityEnabled;

	j[ "lockRot" ][ "x" ] = m_LockRotation[0];
	j[ "lockRot" ][ "y" ] = m_LockRotation[1];
	j[ "lockRot" ][ "z" ] = m_LockRotation[2];
}

void Pine::RigidBody::LoadFromJson( nlohmann::json& j )
{
	SetRigidBodyType( static_cast< Pine::RigidBodyType >( j[ "rgType" ].get<int>( ) ) );

	m_Mass = j[ "mass" ];
	m_GravityEnabled = j[ "gvEnabled" ];

    if (j.contains("lockRot"))
    {
        m_LockRotation[0] = j[ "lockRot" ][ "x" ];
        m_LockRotation[1] = j[ "lockRot" ][ "y" ];
        m_LockRotation[2] = j[ "lockRot" ][ "z" ];
    }
}

void Pine::RigidBody::UpdateRigidbodyProperties( )
{
    const auto transform = GetParent( )->GetTransform( );

    reactphysics3d::Transform tr;

    const auto quat = transform->GetRotationSum();

    tr.setPosition( reactphysics3d::Vector3( transform->Position.x, transform->Position.y, transform->Position.z ) );
    tr.setOrientation( reactphysics3d::Quaternion( quat.x, quat.y, quat.z, quat.w ) );

    m_RigidBody->setMass( m_Mass );
    m_RigidBody->enableGravity( m_GravityEnabled );
    m_RigidBody->setTransform( tr );

    m_RigidBody->setAngularLockAxisFactor(reactphysics3d::Vector3(m_LockRotation[0] ? 0.f : 1.f, m_LockRotation[1] ? 0.f : 1.f, m_LockRotation[2] ? 0.f : 1.f));

    m_RigidBody->setLinearDamping(.2f);

    switch ( m_RigidBodyType )
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

    UpdateColliders( );
}

std::array<bool, 3> Pine::RigidBody::GetRotationLock() {
    return m_LockRotation;
}

void Pine::RigidBody::SetRotationLock(std::array<bool, 3> rot) {
    m_LockRotation = rot;
}
