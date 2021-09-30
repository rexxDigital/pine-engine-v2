#pragma once

#include "IComponent/IComponent.hpp"
#include "Transform/Transform.hpp"
#include "ModelRenderer/ModelRenderer.hpp"
#include "Camera/Camera.hpp"
#include "Light/Light.hpp"
#include "NativeScript/NativeScript.hpp"
#include "Collider3D/Collider3D.hpp"
#include "RigidBody/RigidBody.hpp"

// To query stuff about components
namespace Pine::Components
{
	void Setup( );
	void Dispose( );

	// Enumerate component types
	const char* GetComponentTypeName( EComponentType type );
	int GetComponentTypeCount( );

	// Enumerate components within types
	int GetComponentCount( EComponentType type );
	IComponent* GetComponent( EComponentType type, int index );

	// Manage components
	IComponent* CreateComponent( EComponentType type, bool standalone = false );
	IComponent* CopyComponent( const IComponent* component, bool standalone );
	bool DeleteComponent( IComponent* component );


	namespace Internal
	{
		// If you would like to register your own component, that's (NOT RIGHT NOW SORRY!) possible.
		void RegisterComponent( IComponent* component, const size_t componentSize, const char* str );

		void ShowDebugUI( );
	}

}