#include "Components.hpp"

#include "Behavior/Behavior.hpp"
#include "TerrainRenderer/TerrainRenderer.hpp"

#include "../Core/Log/Log.hpp"

// WARNING: This entire shit might break when adding new elements to g_Components and calling CreateComponent()
// because of g_Components having to resize.

// Creating and removing component code could be optimized as well.

namespace {

	struct Component_t
	{
		Pine::IComponent* m_Component;
		size_t m_ComponentSize;

		std::string m_Name;

		void* m_Data = nullptr;
		size_t m_DataSize = 0;

		bool* m_DataValid = nullptr;
		size_t m_DataValidSize = 0;

	};

	std::vector<Component_t> g_Components;

	int FindAvailableDataSlot( Component_t* comp )
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
		bool* dataValid = static_cast<bool*>( malloc( newSize ) );

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

	void RegisterInternalComponents( )
	{
		using namespace Pine;
		using namespace Pine::Components::Internal;

		RegisterComponent( nullptr, 0, "Invalid" );
		RegisterComponent( new Transform( ), sizeof( Transform ), "Transform" );
		RegisterComponent( new ModelRenderer( ), sizeof( ModelRenderer ), "ModelRenderer" );
		RegisterComponent( new Camera( ), sizeof( Camera ), "Camera" );
		RegisterComponent( new Light( ), sizeof( Light ), "Light" );
		RegisterComponent( nullptr, sizeof( NativeScript ), "Native Script" );
		RegisterComponent( new Behavior( ), sizeof( Behavior ), "Behavior" );
		RegisterComponent( new TerrainRenderer( ), sizeof( TerrainRenderer ), "Terrain Renderer" );
	}
}

void Pine::Components::Setup( )
{
	RegisterInternalComponents( );
}

void Pine::Components::Dispose( )
{
	for ( const auto& component : g_Components )
	{
		if ( component.m_Data )
			free( component.m_Data );
		if ( component.m_DataValid )
			free( component.m_DataValid );
	}
}

int Pine::Components::GetComponentCount( ) {
	return static_cast<int>( g_Components.size( ) );
}

const char* Pine::Components::GetComponentName( EComponentType type ) {
	return g_Components[ static_cast<int>( type ) ].m_Name.c_str( );
}

Pine::IComponent* Pine::Components::CreateComponent( EComponentType type ) {

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

	const int slot = FindAvailableDataSlot( comp );

	if ( slot == -1 ) // TODO: Resize vector or something
	{
		Pine::Log::Warning( "Failed to find available slot for component!" );
		return nullptr;
	}

	// How do you do this properly?
	const auto componentPtr = reinterpret_cast<IComponent*>( reinterpret_cast<std::uintptr_t>( comp->m_Data ) + ( comp->m_ComponentSize * slot ) );

	memcpy_s( componentPtr, comp->m_ComponentSize, comp->m_Component, comp->m_ComponentSize );

	comp->m_DataValid[ slot ] = true;

	return componentPtr;
}

bool Pine::Components::DeleteComponent( IComponent* inputComponent )
{
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

	for ( int i = 0; i < comp->m_DataValidSize;i++ )
	{
		const auto componentPtr = reinterpret_cast<IComponent*>( reinterpret_cast<std::uintptr_t>( comp->m_Data ) + ( comp->m_ComponentSize * i ) );

		if ( componentPtr == inputComponent )
		{
			// Just mark it as invalid and it will probably get overwritten sooner or later.
			comp->m_DataValid[ i ] = false;

			return true;
		}
	}

	return false;
}

void Pine::Components::Internal::RegisterComponent( IComponent* component, const size_t componentSize, const std::string& str ) {
	Component_t comp;

	comp.m_Component = component;
	comp.m_Name = str;
	comp.m_ComponentSize = componentSize;

	ResizeData( &comp, componentSize, 128 ); // By default make space for 128 components

	g_Components.push_back( comp );
}
