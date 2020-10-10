#pragma once

#include "IComponent/IComponent.hpp"
#include "Transform/Transform.hpp"
#include "ModelRenderer/ModelRenderer.hpp"
#include "Camera/Camera.hpp"
#include "Light/Light.hpp"
#include "NativeScript/NativeScript.hpp"

// To query stuff about components
namespace Pine::Components
{
	int GetComponentCount( );

	const char* GetComponentName( EComponentType type );
	IComponent* CreateComponent( EComponentType type );

	namespace Internal
	{
		// Register all the "out of the box" components, such as transform, model renderer etc.
		void RegisterPineComponents( );

		// If you would like to register your own component, that's possible.
		void RegisterComponent( IComponent* component );	
	}

}