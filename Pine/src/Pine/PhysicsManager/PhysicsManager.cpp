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
        double m_PhysicsTotalTime = 0.0f;
	public:

		void Setup( ) override
		{
			m_PhysicsCommon = new reactphysics3d::PhysicsCommon;

			m_PhysicsWorld = m_PhysicsCommon->createPhysicsWorld( );
            m_PhysicsWorld->setIsDebugRenderingEnabled(false);
            m_PhysicsWorld->enableSleeping(true);
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

            Timer physTotalTime;

			constexpr float timeStep = 1.0 / 60.0; // we'll target 60 for now

			accumulator += deltaTime;

            if (accumulator <= timeStep)
            {
                return;
            }

            const double physicsTimeDelta = accumulator;

            accumulator = 0;

            Timer prePhysTimer;

			const auto rigidBodyData = Components->GetComponentData( ComponentType::RigidBody );
			const auto collider3DData = Components->GetComponentData( ComponentType::Collider3D );

			// Call pre-physics update

			for ( int i = 0; i < rigidBodyData->m_DataValidSize; i++ ) // Rigid body
			{
                if (!rigidBodyData->m_DataValid[i]) continue;

				const auto component = rigidBodyData->GetComponent( i );

				if ( !component )
					continue;

				dynamic_cast< RigidBody* >( component )->OnPrePhysicsUpdate( );
			}

            for ( int i = 0; i < collider3DData->m_DataValidSize; i++ ) // Rigid body
            {
                if (!collider3DData->m_DataValid[i]) continue;

                const auto component = collider3DData->GetComponent( i );

				if ( !component )
					continue;

				dynamic_cast< Collider3D* >( component )->OnPrePhysicsUpdate( );
			}

            prePhysTimer.Stop();
            m_PrePhysicsTime = prePhysTimer.GetElapsedTimeInMs();

            Timer physTime;

            m_PhysicsWorld->update( physicsTimeDelta );

            physTime.Stop();
            m_PhysicsTime = physTime.GetElapsedTimeInMs();

			// Call post-physics update
            Timer postPhys;

            for ( int i = 0; i < rigidBodyData->m_DataValidSize; i++ ) // Rigid body
            {
                if (!rigidBodyData->m_DataValid[i]) continue;

                const auto component = rigidBodyData->GetComponent( i );

				if ( !component )
					continue;

				dynamic_cast< Pine::RigidBody* >( component )->OnPostPhysicsUpdate( );
			}

            postPhys.Stop();
            physTotalTime.Stop();

            m_PostPhysicsTime = postPhys.GetElapsedTimeInMs();
            m_PhysicsTotalTime = physTotalTime.GetElapsedTimeInMs();
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

        double GetPhysicsTotalProcessTime( ) override
        {
            return m_PhysicsTotalTime;
        }

	};

	IPhysicsManager* CreatePhysicsManagerInterface( )
	{
		return new CPhysicsManager;
	}

}
