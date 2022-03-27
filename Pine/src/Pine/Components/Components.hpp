#pragma once

#include "IComponent/IComponent.hpp"
#include "../Core/Interfaces/Interfaces.hpp"

namespace Pine
{

	class IComponents : public IInterface
	{
	public:

		// Enumerate component types
		virtual const char* GetComponentTypeName( ComponentType type ) = 0;
		virtual int GetComponentTypeCount( ) = 0;

		// Enumerate components within types
		virtual int GetComponentCount( ComponentType type ) = 0;
		virtual IComponent* GetComponent( ComponentType type, int index ) = 0;

		// Manage components
		virtual IComponent* CreateComponent( ComponentType type, bool standalone = false ) = 0;
		virtual IComponent* CopyComponent( const IComponent* component, Pine::Entity* newParent, bool standalone ) = 0;
		virtual bool DeleteComponent( IComponent* component ) = 0;

		// If you would like to register your own component, that's (NOT RIGHT NOW SORRY!) possible.
		virtual void RegisterComponent( IComponent* component, const size_t componentSize, const char* str ) = 0;

	};

}