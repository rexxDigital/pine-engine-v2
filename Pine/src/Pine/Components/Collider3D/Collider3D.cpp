#include "Collider3D.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../Entity/Entity.hpp"
#include "../../PhysicsManager/PhysicsManager.hpp"
#include "../../Core/Serialization/Serialization.hpp"
#include "../../Core/Log/Log.hpp"
#include "../RigidBody/RigidBody.hpp"

namespace Pine
{
	class RigidBody;
}

void Pine::Collider3D::UpdateBody( )
{
	const bool hasRigidBody = m_Parent->GetComponent<RigidBody>( ) != nullptr;

	if ( hasRigidBody )
	{
		if ( m_CollisionBody )
		{
			PhysicsManager->GetPhysicsWorld( )->destroyCollisionBody( m_CollisionBody );

			m_Collider = nullptr;
			m_CollisionBody = nullptr;
		}
	}
	else
	{
		m_ShapeUpdated = false;

		if ( !m_CollisionBody )
		{
			m_CollisionBody = PhysicsManager->GetPhysicsWorld( )->createCollisionBody( m_CollisionBodyTransform );
		}

		if ( !m_Collider && m_Shape )
		{
			m_Collider = m_CollisionBody->addCollider( m_Shape, m_CollisionTransform );
		}
	}
}

void Pine::Collider3D::CreateShape( )
{
	switch ( m_Type )
	{
	case ColliderType::Box:
		m_Shape = PhysicsManager->GetPhysicsCommon( )->createBoxShape( reactphysics3d::Vector3( m_Size.x, m_Size.y, m_Size.z ) );
		break;
	case ColliderType::Sphere:
		m_Shape = PhysicsManager->GetPhysicsCommon( )->createSphereShape( m_Size.x );
		break;
	case ColliderType::Capsule:
		m_Shape = PhysicsManager->GetPhysicsCommon( )->createCapsuleShape( m_Size.x, m_Size.y );
		break;
	case ColliderType::ConcaveMesh:
	case ColliderType::ConvexMesh:
	case ColliderType::Heightfield:
		break;
	default:
		break;
	}

	m_ShapeUpdated = true;
}

void Pine::Collider3D::DisposeShape( )
{
	if ( !m_Shape )
		return;

	switch ( m_Type )
	{
	case ColliderType::Box:
		PhysicsManager->GetPhysicsCommon( )->destroyBoxShape( dynamic_cast< reactphysics3d::BoxShape* >( m_Shape ) );
		break;
	case ColliderType::Sphere:
		PhysicsManager->GetPhysicsCommon( )->destroySphereShape( dynamic_cast< reactphysics3d::SphereShape* >( m_Shape ) );
		break;
	case ColliderType::Capsule:
		PhysicsManager->GetPhysicsCommon( )->destroyCapsuleShape( dynamic_cast< reactphysics3d::CapsuleShape* >( m_Shape ) );
		break;
	case ColliderType::ConcaveMesh:
		PhysicsManager->GetPhysicsCommon( )->destroyConcaveMeshShape( dynamic_cast< reactphysics3d::ConcaveMeshShape* >( m_Shape ) );
		break;
	case ColliderType::ConvexMesh:
		PhysicsManager->GetPhysicsCommon( )->destroyConvexMeshShape( dynamic_cast< reactphysics3d::ConvexMeshShape* >( m_Shape ) );
		break;
	case ColliderType::Heightfield:
		PhysicsManager->GetPhysicsCommon( )->destroyHeightFieldShape( dynamic_cast< reactphysics3d::HeightFieldShape* >( m_Shape ) );
		break;
	default:
		break;
	}

	m_Shape = nullptr;
	m_Collider = nullptr;
	m_ShapeUpdated = true;
}

void Pine::Collider3D::UpdateShape( )
{
	auto size = m_Size;

	size *= m_Parent->GetTransform( )->Scale;

	if ( !m_Shape )
		CreateShape( );

	switch ( m_Type )
	{
	case ColliderType::Box:
		dynamic_cast< reactphysics3d::BoxShape* >( m_Shape )->setHalfExtents( reactphysics3d::Vector3( size.x, size.y, size.z ) );
		break;
	case ColliderType::Sphere:
		dynamic_cast< reactphysics3d::SphereShape* >( m_Shape )->setRadius( size.x );
		break;
	case ColliderType::Capsule:
		dynamic_cast< reactphysics3d::CapsuleShape* >( m_Shape )->setRadius( size.x );
		dynamic_cast< reactphysics3d::CapsuleShape* >( m_Shape )->setHeight( size.y );
		break;
	case ColliderType::ConcaveMesh:
	case ColliderType::ConvexMesh:
	case ColliderType::Heightfield:
		break;
	default:
		break;
	}
}

Pine::Collider3D::Collider3D( )
{
	m_ComponentType = ComponentType::Collider3D;
}

void Pine::Collider3D::SetColliderType( ColliderType type )
{
	if ( m_Shape )
	{
		DisposeShape( );
	}

	m_Type = type;

	if ( !m_Standalone )
		CreateShape( );
}

Pine::ColliderType Pine::Collider3D::GetColliderType( ) const
{
	return m_Type;
}

void Pine::Collider3D::SetPosition( glm::vec3 position )
{
	m_Position = position;
}

const glm::vec3& Pine::Collider3D::GetPosition( ) const
{
	return m_Position;
}

void Pine::Collider3D::SetSize( glm::vec3 size )
{
	m_Size = size;
}

const glm::vec3& Pine::Collider3D::GetSize( ) const
{
	return m_Size;
}

void Pine::Collider3D::SetRadius( float radius )
{
	m_Size.x = radius;
}

float Pine::Collider3D::GetRadius( ) const
{
	return m_Size.x;
}

void Pine::Collider3D::SetHeight( float height )
{
	m_Size.y = height;
}

float Pine::Collider3D::GetHeight( ) const
{
	return m_Size.y;
}

bool Pine::Collider3D::PollShapeUpdated( )
{
	const bool ret = m_ShapeUpdated;

	m_ShapeUpdated = false;

	return ret;
}

reactphysics3d::CollisionShape* Pine::Collider3D::GetCollisionShape( ) const
{
	return m_Shape;
}

reactphysics3d::Collider* Pine::Collider3D::GetCollider( ) const
{
	return m_Collider;
}

void Pine::Collider3D::RegisterRigidbodyCollider( reactphysics3d::Collider* collider )
{
	m_RigidbodyColliders.push_back( collider );
}

void Pine::Collider3D::OnCreated( )
{
	if ( m_Standalone )
		return;
}

void Pine::Collider3D::OnCopied( const IComponent* old )
{
	m_CollisionBody = nullptr;
	m_Shape = nullptr;
}

void Pine::Collider3D::OnDestroyed( )
{
	Log->Warning( "Collider3D::OnDestroyed( ) -> (" + m_Parent->GetName(  ) + ")" );

	//const auto rigidBody = m_Parent->GetComponent<Pine::RigidBody>( );
	//if ( rigidBody )
	//{
	//	if ( rigidBody->IsColliderAttatched( this ) )
	//	{
	//		rigidBody->DetachCollider( );
	//	}
	//}

	if ( m_CollisionBody )
	{
		PhysicsManager->GetPhysicsWorld( )->destroyCollisionBody( m_CollisionBody );

		m_CollisionBody = nullptr;
	}

	DisposeShape( );
}

void Pine::Collider3D::OnPrePhysicsUpdate( )
{
	if ( m_Standalone ) return;

	UpdateShape( );
	UpdateBody( );

	if ( !m_Shape )
	{
		Log->Warning( "Pine::Collider3D::OnPrePhysicsUpdate( ): No collision shape?" );
		return;
	}

	if ( m_CollisionBody )
	{
		const auto transform = GetParent( )->GetTransform( );
		const auto rotQuat = glm::quat( transform->Rotation );

		reactphysics3d::Transform tr;

		const auto rotRadians = glm::radians( transform->Rotation );

		tr.setPosition( reactphysics3d::Vector3( transform->Position.x, transform->Position.y, transform->Position.z ) );
		tr.setOrientation( reactphysics3d::Quaternion::fromEulerAngles( reactphysics3d::Vector3( rotRadians.x, rotRadians.y, rotRadians.z ) ) );

		m_CollisionBody->setTransform( tr );
	}
}

void Pine::Collider3D::OnSetup( )
{

}

void Pine::Collider3D::OnUpdate( float deltaTime )
{

}

void Pine::Collider3D::SaveToJson( nlohmann::json& j )
{
	j[ "colliderType" ] = m_Type;

	Serialization::SaveVec3( j[ "position" ], m_Position );
	Serialization::SaveVec3( j[ "size" ], m_Size );
}

void Pine::Collider3D::LoadFromJson( nlohmann::json& j )
{
	SetColliderType( j[ "colliderType" ] );

	m_Position = Serialization::LoadVec3( j, "position" );
	m_Size = Serialization::LoadVec3( j, "size" );
}
