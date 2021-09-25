#include "PhysicsManager.hpp"
#include "../Components/Components.hpp"

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
//	m_PhysicsCommon->destroyPhysicsWorld( m_PhysicsWorld );

	delete m_PhysicsCommon;
}

void Pine::PhysicsManager::Update( const double deltaTime )
{
	static double accumulator = 0.0;
	constexpr double timeStep = 1.0f / 60.0f;

	accumulator += deltaTime;

	const auto colliderCount = Components::GetComponentCount( EComponentType::Collider );

	// Call pre-physics update
	for ( int i = 0; i < colliderCount;i++ )
	{
		dynamic_cast<Pine::Collider*>( Components::GetComponent( EComponentType::Collider, i ) )->OnPrePhysicsUpdate( );
	}

	while ( accumulator >= timeStep ) {
		m_PhysicsWorld->update( timeStep );

		accumulator -= timeStep;
	}

	// Call post-physics update
	for ( int i = 0; i < colliderCount; i++ )
	{
		dynamic_cast<Pine::Collider*>( Components::GetComponent( EComponentType::Collider, i ) )->OnPostPhysicsUpdate( );
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
