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
	void Setup( );
	void Dispose( );

	const char* GetComponentName( EComponentType type );
	int GetComponentCount( );

	IComponent* CreateComponent( EComponentType type, bool standalone = false );
	bool DeleteComponent( IComponent* component );

	IComponent* CopyComponent( const IComponent* component, bool standalone );

	namespace Internal
	{
		// If you would like to register your own component, that's (NOT RIGHT NOW SORRY!) possible.
		void RegisterComponent( IComponent* component, const size_t componentSize, const std::string& str );	
	}

}