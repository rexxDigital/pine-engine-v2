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

	// These are quick wrappers for creating and destroying stuff within the physics library,
	// they are used so we can more easily keep track of allocations.

	reactphysics3d::RigidBody* CreateRigidBody( const reactphysics3d::Transform& transform );
	void DestroyRigidBody( reactphysics3d::RigidBody* body );

}