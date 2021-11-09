#pragma once

#include "IComponent/IComponent.hpp"
#include "Transform/Transform.hpp"
#include "ModelRenderer/ModelRenderer.hpp"
#include "Camera/Camera.hpp"
#include "Light/Light.hpp"
#include "NativeScript/NativeScript.hpp"
#include "Collider3D/Collider3D.hpp"
#include "RigidBody/RigidBody.hpp"

namespace Pine
{

	class IComponents : public IInterface
	{
	public:

		// Enumerate component types
		virtual const char* GetComponentTypeName( EComponentType type ) = 0;
		virtual int GetComponentTypeCount( ) = 0;

		// Enumerate components within types
		virtual int GetComponentCount( EComponentType type ) = 0;
		virtual IComponent* GetComponent( EComponentType type, int index ) = 0;

		// Manage components
		virtual IComponent* CreateComponent( EComponentType type, bool standalone = false ) = 0;
		virtual IComponent* CopyComponent( const IComponent* component, bool standalone ) = 0;
		virtual bool DeleteComponent( IComponent* component ) = 0;

		// If you would like to register your own component, that's (NOT RIGHT NOW SORRY!) possible.
		virtual void RegisterComponent( IComponent* component, const size_t componentSize, const char* str ) = 0;

	};

}