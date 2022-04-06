#pragma once

#include "IComponent/IComponent.hpp"
#include "../Core/Interfaces/Interfaces.hpp"

namespace Pine
{

    struct ComponentData_t
    {
        Pine::IComponent* m_Component;
        size_t m_ComponentSize;

        const char* m_Name;

        void* m_Data = nullptr;
        size_t m_DataSize = 0;

        bool* m_DataValid = nullptr;
        size_t m_DataValidSize = 0;

        __forceinline IComponent* GetComponent( int index )
        {
            return reinterpret_cast< IComponent* >( reinterpret_cast< std::uintptr_t >( m_Data ) + ( m_ComponentSize * index ) );
        }
    };

    class IComponents : public IInterface
	{
	public:

		// Enumerate component types
		virtual const char* GetComponentTypeName( ComponentType type ) = 0;
		virtual int GetComponentTypeCount( ) = 0;

		// Enumerate components within types
		virtual int GetComponentCount( ComponentType type ) = 0;

        // Please don't enumerate through every component wit this, since it would be very slow.
        // Call GetComponentData() and enumerate manually instead, use ComponentData_t::m_DataValidSize for count, ComponentData_t::m_DataValid[i] and ComponentData_t::GetComponent(i)
		virtual IComponent* GetComponent( ComponentType type, int index ) = 0;
		virtual ComponentData_t* GetComponentData( ComponentType type ) = 0;

		// Manage components
		virtual IComponent* CreateComponent( ComponentType type, bool standalone = false ) = 0;
		virtual IComponent* CopyComponent( const IComponent* component, Pine::Entity* newParent, bool standalone ) = 0;
		virtual bool DeleteComponent( IComponent* component ) = 0;

		// If you would like to register your own component, that's (NOT RIGHT NOW SORRY!) possible.
		virtual void RegisterComponent( IComponent* component, const size_t componentSize, const char* str ) = 0;

	};

}