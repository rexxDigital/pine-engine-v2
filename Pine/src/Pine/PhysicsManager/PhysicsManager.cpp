#include "PhysicsManager.hpp"

#include <random>

#include "../Components/Components.hpp"
#include "../Pine.hpp"
#include "../Core/Timer/Timer.hpp"

#include "../Components/Collider3D/Collider3D.hpp"
#include "../Components/RigidBody/RigidBody.hpp"

namespace Pine
{

	class CPhysicsManager : public IPhysicsManager
	{
	private:
		reactphysics3d::PhysicsCommon* m_PhysicsCommon;
		reactphysics3d::PhysicsWorld* m_PhysicsWorld;

        double m_PrePhysicsTime = 0.0f;
        double m_PhysicsTime = 0.0f;
        double m_PostPhysicsTime = 0.0f;
	public:

		void Setup( ) override
		{
			m_PhysicsCommon = new reactphysics3d::PhysicsCommon;

			m_PhysicsWorld = m_PhysicsCommon->createPhysicsWorld( );
            m_PhysicsWorld->setIsDebugRenderingEnabled(false);
		}

		void Dispose( ) override
		{
			delete m_PhysicsCommon;
		}

		void Update( const double deltaTime ) override
		{
            static double accumulator = 0.0;

            if ( !IsAllowingUpdates( ) )
            {
                accumulator = 0;
                return;
            }

			constexpr float timeStep = 1.0 / 60.0; // we'll target 60 for now

			accumulator += deltaTime;

            Timer prePhysTimer;

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

            prePhysTimer.Stop();
            m_PrePhysicsTime = prePhysTimer.GetElapsedTimeInMs();

            Timer physTime;

            m_PhysicsWorld->update( deltaTime );

            physTime.Stop();
            m_PhysicsTime = physTime.GetElapsedTimeInMs();

			// Call post-physics update
            Timer postPhys;

			for ( int i = 0; i < rigidBodyCount; i++ ) // Rigidbody
			{
				const auto component = Components->GetComponent( ComponentType::RigidBody, i );

				if ( !component )
					continue;

				dynamic_cast< Pine::RigidBody* >( component )->OnPostPhysicsUpdate( );
			}

            postPhys.Stop();
            m_PostPhysicsTime = postPhys.GetElapsedTimeInMs();
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

        double GetPrePhysicsTime( ) override
        {
            return m_PrePhysicsTime;
        }

        double GetPhysicsProcessTime( ) override
        {
            return m_PhysicsTime;
        }

        double GetPostPhysicsTime( ) override
        {
            return m_PostPhysicsTime;
        }

	};

	IPhysicsManager* CreatePhysicsManagerInterface( )
	{
		return new CPhysicsManager;
	}

}
