#pragma once
#include "../IComponent/IComponent.hpp"
#include <reactphysics3d/reactphysics3d.h>

namespace Pine
{

	enum class ColliderType
	{
		Invalid,
		Box,
		Sphere,
		Capsule,
		ConvexMesh,
		ConcaveMesh,
		Heightfield
	};

	class Collider : public IComponent
	{
	private:
		ColliderType m_Type = ColliderType::Invalid;

		reactphysics3d::Transform m_PhysicsTransform;
		reactphysics3d::RigidBody* m_RigidBody = nullptr;

		// A pointer to the underlying shape, t.ex BoxCollider
		reactphysics3d::CollisionShape* m_Shape = nullptr;
	public:
		Collider( );

		void SetColliderType( ColliderType type );
		ColliderType GetColliderType( ) const;

		void OnSetup( ) override;
		void OnUpdate( float deltaTime ) override;

		void OnPrePhysicsUpdate( );
		void OnPostPhysicsUpdate( ) const;

		void OnCreated( ) override;
		void OnDestroyed( ) override;

		void SaveToJson( nlohmann::json& j ) override;
		void LoadFromJson( nlohmann::json& j ) override;
	};


}
