#include "PhysicsManager.hpp"

#include <random>

#include "../Components/Components.hpp"
#include "../Pine.hpp"

#include "../Components/Collider3D/Collider3D.hpp"
#include "../Components/RigidBody/RigidBody.hpp"

namespace Pine
{

	class CPhysicsManager : public IPhysicsManager
	{
	private:
		reactphysics3d::PhysicsCommon* m_PhysicsCommon;
		reactphysics3d::PhysicsWorld* m_PhysicsWorld;
	public:

		void Setup( ) override
		{
			m_PhysicsCommon = new reactphysics3d::PhysicsCommon;

			m_PhysicsWorld = m_PhysicsCommon->createPhysicsWorld( );
		}

		void Dispose( ) override
		{
			delete m_PhysicsCommon;
		}

		void Update( const double deltaTime ) override
		{
			if ( !IsAllowingUpdates( ) )
				return;

			static double accumulator = 0.0;
			constexpr float timeStep = 1.0 / 60.0; // we'll target 60 for now

			accumulator += deltaTime;

			const auto rigidBodyCount = Components->GetComponentCount( ComponentType::RigidBody );
			const auto collider3DCount = Components->GetComponentCount( ComponentType::Collider3D );

			// Call pre-physics update

			for ( int i = 0; i < rigidBodyCount; i++ ) // Rigid body
			{
				const auto component = Components->GetComponent( ComponentType::RigidBody, i );

				if ( !component )
					continue;

				dynamic_cast< RigidBody* >( component )->OnPrePhysicsUpdate( );
			}

			for ( int i = 0; i < collider3DCount; i++ ) // Collider 3D
			{
				const auto component = Components->GetComponent( ComponentType::Collider3D, i );

				if ( !component )
					continue;

				dynamic_cast< Collider3D* >( component )->OnPrePhysicsUpdate( );
			}

			// Run physics simulation:
			while ( accumulator >= timeStep ) 
			{
				m_PhysicsWorld->update( timeStep );

				accumulator -= timeStep;
			}

			// Call post-physics update

			for ( int i = 0; i < rigidBodyCount; i++ ) // Rigidbody
			{
				const auto component = Components->GetComponent( ComponentType::RigidBody, i );

				if ( !component )
					continue;

				dynamic_cast< Pine::RigidBody* >( component )->OnPostPhysicsUpdate( );
			}
		}

		reactphysics3d::PhysicsCommon* GetPhysicsCommon( ) override
		{
			return m_PhysicsCommon;
		}

		reactphysics3d::PhysicsWorld* GetPhysicsWorld( ) override
		{
			return m_PhysicsWorld;
		}

		reactphysics3d::RigidBody* CreateRigidBody( const reactphysics3d::Transform& transform ) override
		{
			return m_PhysicsWorld->createRigidBody( transform );
		}

		void DestroyRigidBody( reactphysics3d::RigidBody* body ) override
		{
			m_PhysicsWorld->destroyRigidBody( body );
		}

	};

	IPhysicsManager* CreatePhysicsManagerInterface( )
	{
		return new CPhysicsManager;
	}

}
