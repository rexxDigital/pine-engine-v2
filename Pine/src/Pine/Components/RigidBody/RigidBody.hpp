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
        std::array<bool, 3> m_LockRotation = {false, false, false};

		RigidBodyType m_RigidBodyType = RigidBodyType::Kinematic;

		// TODO: Add support for multiple colliders

		reactphysics3d::Collider* m_Collider = nullptr;
		Pine::Collider3D* m_Collider3D = nullptr;

		void UpdateColliders( );

        void UpdateRigidbodyProperties( );
	public:
		RigidBody( );

		reactphysics3d::RigidBody* GetRigidBody( ) const;

        void SetRotationLock(std::array<bool, 3> rot);
        std::array<bool, 3> GetRotationLock( );

		void SetMass( float mass );
		float GetMass( ) const;

		void SetGravityEnabled( bool value );
		bool GetGravityEnabled( ) const;

		void SetRigidBodyType( RigidBodyType type );
		RigidBodyType GetRigidBodyType( ) const;

		// This will detach the collider3d from this rigidbody for 1 physics update only.
		void DetachCollider( );

		bool IsColliderAttached( Collider3D* collider ) const;

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