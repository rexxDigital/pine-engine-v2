#include "Components.hpp"

#include "TerrainRenderer/TerrainRenderer.hpp"

#include "../Core/Log/Log.hpp"

#include "../../ImGui/imgui.h"

#include "Transform/Transform.hpp"
#include "ModelRenderer/ModelRenderer.hpp"
#include "Camera/Camera.hpp"
#include "Light/Light.hpp"
#include "NativeScript/NativeScript.hpp"
#include "Collider3D/Collider3D.hpp"
#include "RigidBody/RigidBody.hpp"
#include "../Entity/Entity.hpp"

namespace
{

	struct Component_t
	{
		Pine::IComponent* m_Component;
		size_t m_ComponentSize;

		const char* m_Name;

		void* m_Data = nullptr;
		size_t m_DataSize = 0;

		bool* m_DataValid = nullptr;
		size_t m_DataValidSize = 0;
	};

	int FindAvailableDataSlot( const Component_t* comp )
	{
		for ( int i = 0; i < comp->m_DataValidSize; i++ )
		{
			if ( !comp->m_DataValid[ i ] )
				return i;
		}

		return -1; // ran out of data?
	}

	void ResizeData( Component_t* comp, const size_t elementSize, const size_t newSize )
	{
		if ( elementSize == 0 )
			return;

		void* data = malloc( elementSize * newSize );
		bool* dataValid = static_cast< bool* >( malloc( newSize ) );

		if ( !data || !dataValid )
			return;

		memset( data, 0, elementSize * newSize );
		memset( dataValid, 0, newSize );

		// Attempt to copy and free old data
		if ( comp->m_Data )
		{
			if ( elementSize * newSize > comp->m_DataSize )
				memcpy_s( data, elementSize * newSize, comp->m_Data, comp->m_DataSize );

			free( comp->m_Data );
		}

		// Attempt to copy and free old valid ptr
		if ( comp->m_DataValid )
		{
			if ( newSize > comp->m_DataValidSize )
			{
				memcpy_s( dataValid, elementSize, comp->m_DataValid, comp->m_DataValidSize );
			}

			free( comp->m_DataValid );
		}

		comp->m_Data = data;
		comp->m_DataSize = elementSize * newSize;

		comp->m_DataValid = dataValid;
		comp->m_DataValidSize = newSize;
	}

}

namespace Pine
{

	class CComponents : public IComponents
	{
	private:

		std::vector<Component_t> g_Components;

		void RegisterInternalComponents( )
		{
			RegisterComponent( nullptr, 0, "Invalid" );
			RegisterComponent( new Transform( ), sizeof( Transform ), "Transform" );
			RegisterComponent( new ModelRenderer( ), sizeof( ModelRenderer ), "Model Renderer" );
			RegisterComponent( new Camera( ), sizeof( Camera ), "Camera" );
			RegisterComponent( new Light( ), sizeof( Light ), "Light" );
			RegisterComponent( new NativeScript( ), sizeof( NativeScript ), "Native Script" );
			RegisterComponent( new TerrainRenderer( ), sizeof( TerrainRenderer ), "Terrain Renderer" );
			RegisterComponent( new Collider3D( ), sizeof( Collider3D ), "Collider3D" );
            RegisterComponent( new RigidBody( ), sizeof( RigidBody ), "Rigid Body" );
        }

	public:

		void Setup( ) override
		{
			Log->Debug( "Pine::Components->Setup( )" );

			RegisterInternalComponents( );
		}

		void Dispose( ) override
		{
			Log->Debug( "Pine::Components->Dispose( )" );

			for ( auto& component : g_Components )
			{
				const int size = FindAvailableDataSlot( &component );

				for ( int i = 0; i < size; i++ )
				{
					const auto componentPtr = reinterpret_cast< IComponent* >( reinterpret_cast< std::uintptr_t >( component.m_Data ) + ( component.m_ComponentSize * i ) );

					if ( component.m_DataValid[ i ] )
						componentPtr->OnDestroyed( );
				}

				if ( component.m_Data )
					free( component.m_Data );
				if ( component.m_DataValid )
					free( component.m_DataValid );
			}
		}

		int GetComponentTypeCount( ) override
		{
			return static_cast< int >( g_Components.size( ) );
		}

		int GetComponentCount( ComponentType type ) override
		{
			Component_t* comp = nullptr;

			for ( auto& component : g_Components )
			{
				if ( !component.m_Component )
					continue;

				if ( component.m_Component->GetType( ) == type )
				{
					comp = &component;
					break;
				}
			}

			if ( !comp )
				return 0;

			int currentCount = 0;

			for ( int i = 0; i < comp->m_DataValidSize; i++ )
			{
				if ( comp->m_DataValid[ i ] )
					currentCount = 1 + i;
			}

			return currentCount;
		}

		IComponent* GetComponent( ComponentType type, int index ) override
		{
			Component_t* comp = nullptr;

			for ( auto& component : g_Components )
			{
				if ( !component.m_Component )
					continue;

				if ( component.m_Component->GetType( ) == type )
				{
					comp = &component;
					break;
				}
			}

			if ( !comp )
				return nullptr;

			if ( !comp->m_DataValid[ index ] )
				return nullptr;

			return reinterpret_cast< IComponent* >( reinterpret_cast< std::uintptr_t >( comp->m_Data ) + ( comp->m_ComponentSize * index ) );
		}

		const char* GetComponentTypeName( ComponentType type ) override
		{
			return g_Components[ static_cast< int >( type ) ].m_Name;
		}

		IComponent* CreateComponent( ComponentType type, bool standalone ) override
		{
			Component_t* comp = nullptr;

			for ( auto& component : g_Components )
			{
				if ( !component.m_Component )
					continue;

				if ( component.m_Component->GetType( ) == type )
				{
					comp = &component;
					break;
				}
			}

			if ( !comp )
			{
				Log->Warning( "Failed to find component type." );

				return nullptr;
			}

			if ( standalone )
			{
				const auto component = static_cast< IComponent* >( malloc( comp->m_ComponentSize ) );

				memcpy_s( component, comp->m_ComponentSize, comp->m_Component, comp->m_ComponentSize );

				component->SetStandalone( true );
				component->OnCreated( );

				return component;
			}

			const int slot = FindAvailableDataSlot( comp );

			if ( slot == -1 ) // TODO: Resize vector or something
			{
				Pine::Log->Warning( "Failed to find available slot for component!" );
				return nullptr;
			}

			const auto componentPtr = reinterpret_cast< IComponent* >( reinterpret_cast< std::uintptr_t >( comp->m_Data ) + ( comp->m_ComponentSize * slot ) );

			memcpy_s( componentPtr, comp->m_ComponentSize, comp->m_Component, comp->m_ComponentSize );

			comp->m_DataValid[ slot ] = true;

			componentPtr->SetStandalone( false );

			Log->Debug( "Pine::Components->CreateComponent( " + std::string( g_Components[ static_cast< int >( type ) ].m_Name ) + ", " + std::to_string( standalone ) + " ): slot -> " + std::to_string( slot ) );

			componentPtr->OnCreated( );

			return componentPtr;
		}

		bool DeleteComponent( IComponent* inputComponent ) override
		{
			// If this is during shutdown, we might have called Pine::Components->Dispose( ) already.
			if ( g_Components.empty( ) )
			{
				return true;
			}

			Log->Debug( "Pine::Components->DeleteComponent( ): standalone -> " + std::to_string( inputComponent->GetStandalone( ) ) );

			inputComponent->OnDestroyed( );

			if ( inputComponent->GetStandalone( ) )
			{
				free( inputComponent );

				return true;
			}

			Component_t* comp = nullptr;

			for ( auto& component : g_Components )
			{
				if ( !component.m_Component )
					continue;

				if ( component.m_Component->GetType( ) == inputComponent->GetType( ) )
				{
					comp = &component;
					break;
				}
			}

			if ( !comp )
				return false;

			for ( int i = 0; i < comp->m_DataValidSize; i++ )
			{
				const auto componentPtr = reinterpret_cast< IComponent* >( reinterpret_cast< std::uintptr_t >( comp->m_Data ) + ( comp->m_ComponentSize * i ) );

				if ( componentPtr == inputComponent )
				{
					// Just mark it as invalid, and it will probably get overwritten sooner or later.
					comp->m_DataValid[ i ] = false;

					return true;
				}
			}

			return false;
		}

		IComponent* CopyComponent( const Pine::IComponent* inputComponent, Pine::Entity* newParent, bool standalone ) override
		{
			Log->Debug( "Pine::Components->CopyComponent( ): standalone -> " + std::to_string( standalone ) );

			Component_t* comp = nullptr;

			for ( auto& component : g_Components )
			{
				if ( !component.m_Component )
					continue;

				if ( component.m_Component->GetType( ) == inputComponent->GetType( ) )
				{
					comp = &component;
					break;
				}
			}

			if ( !comp )
				return nullptr;

			IComponent* component = nullptr;

			if ( standalone )
			{
				component = static_cast< IComponent* >( malloc( comp->m_ComponentSize ) );
			}
			else
			{
				const int slot = FindAvailableDataSlot( comp );

				if ( slot == -1 ) // TODO: Resize vector or something
				{
					Pine::Log->Warning( "Failed to find available slot for component!" );
					return nullptr;
				}

				component = reinterpret_cast< IComponent* >( reinterpret_cast< std::uintptr_t >( comp->m_Data ) + ( comp->m_ComponentSize * slot ) );

				comp->m_DataValid[ slot ] = true;
			}

			// By doing a simple memory copy we'll obviously only copy all the data within the component object itself only, including
			// pointers and such to other objects, but this means it won't copy the data within the pointers, and will probably cause issues
			// for example the physics library.

			memcpy_s( component, comp->m_ComponentSize, inputComponent, comp->m_ComponentSize );

            component->SetParent( newParent );
			component->SetStandalone( standalone );

			component->OnCopied( inputComponent );
			component->OnCreated( );

			return component;
		}

		void RegisterComponent( IComponent* component, const size_t componentSize, const char* str ) override
		{
			Component_t comp;

			comp.m_Component = component;
			comp.m_Name = str;
			comp.m_ComponentSize = componentSize;

			if ( component )
			{
				component->SetStandalone( false );

				ResizeData( &comp, componentSize, 1024 ); // By default make space for 256 components
			}

			g_Components.push_back( comp );
		}
	};

	IComponents* CreateComponentsInterface( )
	{
		return new CComponents( );
	}

}
