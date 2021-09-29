#include "PhysicsManager.hpp"
#include "../Components/Components.hpp"
#include "../Pine.hpp"

namespace
{
	reactphysics3d::PhysicsCommon* m_PhysicsCommon;
	reactphysics3d::PhysicsWorld* m_PhysicsWorld;
}

void Pine::PhysicsManager::Setup( )
{
	m_PhysicsCommon = new reactphysics3d::PhysicsCommon;

	m_PhysicsWorld = m_PhysicsCommon->createPhysicsWorld( );
}

void Pine::PhysicsManager::Dispose( )
{
	delete m_PhysicsCommon;
}

void Pine::PhysicsManager::Update( const double deltaTime )
{
	if ( !Pine::IsAllowingUpdates( ) )
		return;

	static double accumulator = 0.0;
	constexpr double timeStep = 1.0 / 60.0; // we'll target 60 for now

	accumulator += deltaTime;

	const auto colliderCount = Components::GetComponentCount( EComponentType::RigidBody );

	// Call pre-physics update
	for ( int i = 0; i < colliderCount;i++ )
	{
		dynamic_cast<Pine::RigidBody*>( Components::GetComponent( EComponentType::RigidBody, i ) )->OnPrePhysicsUpdate( );
	}

	while ( accumulator >= timeStep ) {
		m_PhysicsWorld->update( timeStep );

		accumulator -= timeStep;
	}

	// Call post-physics update
	for ( int i = 0; i < colliderCount; i++ )
	{
		dynamic_cast<Pine::RigidBody*>( Components::GetComponent( EComponentType::RigidBody, i ) )->OnPostPhysicsUpdate( );
	}
}

reactphysics3d::PhysicsCommon* Pine::PhysicsManager::GetPhysicsCommon( )
{
	return m_PhysicsCommon;
}

reactphysics3d::PhysicsWorld* Pine::PhysicsManager::GetPhysicsWorld( )
{
	return m_PhysicsWorld;
}
