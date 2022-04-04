#pragma once
#include <reactphysics3d/reactphysics3d.h>
#include "../Core/Interfaces/Interfaces.hpp"

namespace Pine
{

	class IPhysicsManager : public IInterface
	{
	private:
	public:
		virtual void Update( const double deltaTime ) = 0;

		virtual reactphysics3d::PhysicsCommon* GetPhysicsCommon( ) = 0;

		// This is the main physics world used by all the entities.
		virtual reactphysics3d::PhysicsWorld* GetPhysicsWorld( ) = 0;

        // Performance wrappers
        virtual double GetPrePhysicsTime( ) = 0;
        virtual double GetPhysicsProcessTime( ) = 0;
        virtual double GetPostPhysicsTime( ) = 0;
        virtual double GetPhysicsTotalProcessTime( ) = 0;

		// These are quick wrappers for creating and destroying stuff within the physics library,
		// they are used so we can more easily keep track of allocations.

		virtual reactphysics3d::RigidBody* CreateRigidBody( const reactphysics3d::Transform& transform ) = 0;
		virtual void DestroyRigidBody( reactphysics3d::RigidBody* body ) = 0;
	};


}
