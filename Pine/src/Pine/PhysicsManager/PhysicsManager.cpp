#include "PhysicsManager.hpp"

#include <random>

#include "../Components/Components.hpp"
#include "../Pine.hpp"

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
			if ( !Pine::IsAllowingUpdates( ) )
				return;

			static double accumulator = 0.0;
			constexpr float timeStep = 1.0 / 60.0; // we'll target 60 for now

			accumulator += deltaTime;

			const auto colliderCount = Components->GetComponentCount( EComponentType::RigidBody );

			// Call pre-physics update
			for ( int i = 0; i < colliderCount; i++ )
			{
				const auto component = Components->GetComponent( EComponentType::RigidBody, i );

				if ( !component )
					continue;

				dynamic_cast< Pine::RigidBody* >( component )->OnPrePhysicsUpdate( );
			}

			while ( accumulator >= timeStep ) {
				m_PhysicsWorld->update( timeStep );

				accumulator -= timeStep;
			}

			// Call post-physics update
			for ( int i = 0; i < colliderCount; i++ )
			{
				const auto component = Components->GetComponent( EComponentType::RigidBody, i );

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
