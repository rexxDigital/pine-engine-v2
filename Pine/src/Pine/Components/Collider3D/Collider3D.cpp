#include "Collider3D.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "../../Entity/Entity.hpp"
#include "../../PhysicsManager/PhysicsManager.hpp"
#include "../../Core/Serialization/Serialization.hpp"
#include "../../Core/Log/Log.hpp"

namespace Pine
{
	class RigidBody;
}

void Pine::Collider3D::UpdateBody( )
{
	const auto rigidBody = m_Parent->GetComponent<Pine::RigidBody>( );

	// If we already have a rigid body, we don't want a collision body.
	if ( rigidBody ) 
	{
		if ( m_Body )
		{
			PhysicsManager::GetPhysicsWorld( )->destroyCollisionBody( m_Body );

			m_Body = nullptr;
		}

		return;
	}

	if ( !m_Body )
	{
		m_Body = PhysicsManager::GetPhysicsWorld( )->createCollisionBody( m_PhysTransform );
	}
}

void Pine::Collider3D::CreateShape( )
{
	switch ( m_Type )
	{
	case ColliderType::Box:
		m_Shape = PhysicsManager::GetPhysicsCommon( )->createBoxShape( reactphysics3d::Vector3( m_Size.x, m_Size.y, m_Size.z ) );
		break;
	case ColliderType::Sphere:
		m_Shape = PhysicsManager::GetPhysicsCommon( )->createSphereShape( m_Size.x );
		break;
	case ColliderType::Capsule:
		m_Shape = PhysicsManager::GetPhysicsCommon( )->createCapsuleShape( m_Size.x, m_Size.y );
		break;
	case ColliderType::ConcaveMesh:
	case ColliderType::ConvexMesh:
	case ColliderType::Heightfield:
		break;
	default:
		break;
	}
}

void Pine::Collider3D::DisposeShape( )
{
	if ( !m_Shape )
		return;

	switch ( m_Type )
	{
	case ColliderType::Box:
		PhysicsManager::GetPhysicsCommon( )->destroyBoxShape( dynamic_cast< reactphysics3d::BoxShape* >( m_Shape ) );
		break;
	case ColliderType::Sphere:
		PhysicsManager::GetPhysicsCommon( )->destroySphereShape( dynamic_cast< reactphysics3d::SphereShape* >( m_Shape ) );
		break;
	case ColliderType::Capsule:
		PhysicsManager::GetPhysicsCommon( )->destroyCapsuleShape( dynamic_cast< reactphysics3d::CapsuleShape* >( m_Shape ) );
		break;
	case ColliderType::ConcaveMesh:
		PhysicsManager::GetPhysicsCommon( )->destroyConcaveMeshShape( dynamic_cast< reactphysics3d::ConcaveMeshShape* >( m_Shape ) );
		break;
	case ColliderType::ConvexMesh:
		PhysicsManager::GetPhysicsCommon( )->destroyConvexMeshShape( dynamic_cast< reactphysics3d::ConvexMeshShape* >( m_Shape ) );
		break;
	case ColliderType::Heightfield:
		PhysicsManager::GetPhysicsCommon( )->destroyHeightFieldShape( dynamic_cast< reactphysics3d::HeightFieldShape* >( m_Shape ) );
		break;
	default:
		break;
	}
}

void Pine::Collider3D::UpdateShape( ) const
{
	switch ( m_Type )
	{
	case ColliderType::Box:
		dynamic_cast< reactphysics3d::BoxShape* >( m_Shape )->setHalfExtents( reactphysics3d::Vector3( m_Size.x, m_Size.y, m_Size.z ) );
		break;
	case ColliderType::Sphere:
		dynamic_cast< reactphysics3d::SphereShape* >( m_Shape )->setRadius( m_Size.x );
		break;
	case ColliderType::Capsule:
		dynamic_cast< reactphysics3d::CapsuleShape* >( m_Shape )->setRadius( m_Size.x );
		dynamic_cast< reactphysics3d::CapsuleShape* >( m_Shape )->setHeight( m_Size.y );
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
	m_ComponentType = EComponentType::Collider3D;
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

void Pine::Collider3D::OnCreated( )
{
	SetColliderType( ColliderType::Box );

	if ( m_Standalone )
		return;

	m_Body = PhysicsManager::CreateRigidBody( m_PhysTransform );
}

void Pine::Collider3D::OnCopied( const IComponent* old )
{
	auto oldCollider = dynamic_cast< const Pine::Collider3D* >( old );
}

void Pine::Collider3D::OnDestroyed( )
{
	PhysicsManager::GetPhysicsWorld( )->destroyCollisionBody( m_Body );
}

void Pine::Collider3D::OnPrePhysicsUpdate( )
{
	if ( m_Standalone ) return;

	UpdateBody( );
	UpdateShape( );

	if ( !m_Shape )
	{
		Pine::Log::Warning( "Pine::Collider3D::OnPrePhysicsUpdate( ): No collision shape?" );
		return;
	}

	if ( m_Body )
	{
		if ( m_Body->getNbColliders(  ) <= 0 )
		{
			m_Body->addCollider( m_Shape, m_PhysTransform );
		}

		glm::vec3 finalPosition = m_Parent->GetTransform(  )->Position;

		finalPosition += m_Position;

		m_PhysTransform.setPosition( reactphysics3d::Vector3( finalPosition.x, finalPosition.y, finalPosition.z ) );

		m_Body->setTransform( m_PhysTransform );
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
	j[ "colliderType" ] = static_cast< int >( m_Type );
	Serialization::SaveVec3( j[ "position" ], m_Position );
	Serialization::SaveVec3( j[ "size" ], m_Size );
}

void Pine::Collider3D::LoadFromJson( nlohmann::json& j )
{
	SetColliderType( static_cast< ColliderType >( j[ "colliderType" ] ) );

	m_Position = Serialization::LoadVec3( j, "position" );
	m_Size = Serialization::LoadVec3( j, "size" );
}
