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

	class RigidBody : public IComponent
	{
	private:
		reactphysics3d::Transform m_PhysicsTransform;
		reactphysics3d::RigidBody* m_RigidBody;

		RigidBodyType m_RigidBodyType = RigidBodyType::Kinematic;
	public:
		RigidBody( );

		reactphysics3d::RigidBody* GetRigidBody( ) const;

		void SetMass( float mass );
		float GetMass( ) const;

		void SetGravityEnabled( bool value );
		bool GetGravityEnabled( ) const;

		void SetRigidBodyType( RigidBodyType type );
		RigidBodyType GetRigidBodyType( ) const;

		void OnPrePhysicsUpdate( ) const;
		void OnPostPhysicsUpdate( ) const;

		void OnCreated( ) override;
		void OnCopied( const IComponent* old ) override;
		void OnDestroyed( ) override;

		void OnSetup( ) override;
		void OnUpdate( float deltaTime ) override;

		void SaveToJson( nlohmann::json& j ) override;
		void LoadFromJson( nlohmann::json& j ) override;
	};

}