#pragma once
#include <reactphysics3d/reactphysics3d.h>

namespace Pine::PhysicsManager
{
	void Setup( );
	void Dispose( );

	void Update( const double deltaTime );

	reactphysics3d::PhysicsCommon* GetPhysicsCommon( );

	// This is the main physics world used by all the entities.
	reactphysics3d::PhysicsWorld* GetPhysicsWorld( );

}