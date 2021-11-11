#pragma once
#include "../IComponent/IComponent.hpp"
#include <reactphysics3d/reactphysics3d.h>

namespace Pine
{
	enum class RigidBodyType
	{
		Static,
		Kinematic,
		Dynamic
	};

	class Collider3D;

	class RigidBody : public IComponent
	{
	private:
		reactphysics3d::Transform m_RigidBodyTransform;
		reactphysics3d::Transform m_ColliderTransform;

		reactphysics3d::RigidBody* m_RigidBody{ };

		float m_Mass = 1.0f;
		bool m_GravityEnabled = true;

		RigidBodyType m_RigidBodyType = RigidBodyType::Kinematic;

		// TODO: Add support for multiple colliders

		reactphysics3d::Collider* m_Collider = nullptr;
		Pine::Collider3D* m_Collider3D = nullptr;

		void UpdateColliders( );
	public:
		RigidBody( );

		reactphysics3d::RigidBody* GetRigidBody( ) const;

		void SetMass( float mass );
		float GetMass( ) const;

		void SetGravityEnabled( bool value );
		bool GetGravityEnabled( ) const;

		void SetRigidBodyType( RigidBodyType type );
		RigidBodyType GetRigidBodyType( ) const;

		void AttachCollider( Collider3D* collider );
		void DetachCollider( Collider3D* collider );

		bool HasColliderAttached( Collider3D* collider ) const;

		void OnPrePhysicsUpdate( );
		void OnPostPhysicsUpdate( );

		void OnCreated( ) override;
		void OnCopied( const IComponent* old ) override;
		void OnDestroyed( ) override;

		void OnSetup( ) override;
		void OnUpdate( float deltaTime ) override;

		void SaveToJson( nlohmann::json& j ) override;
		void LoadFromJson( nlohmann::json& j ) override;
	};

}