#include "Components.hpp"

#include "Behavior/Behavior.hpp"
#include "TerrainRenderer/TerrainRenderer.hpp"

#include "../Core/Log/Log.hpp"

#include "../../ImGui/imgui.h"

namespace {

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
		RegisterComponent( new Collider( ), sizeof( Collider ), "Collider" );
	}
}

void Pine::Components::Setup( )
{
	Log::Debug( "Pine::Components::Setup( )" );

	RegisterInternalComponents( );
}

void Pine::Components::Dispose( )
{
	Log::Debug( "Pine::Components::Dispose( )" );

	for ( const auto& component : g_Components )
	{
		if ( component.m_Data )
			free( component.m_Data );
		if ( component.m_DataValid )
			free( component.m_DataValid );
	}
}

int Pine::Components::GetComponentTypeCount( ) {
	return static_cast<int>( g_Components.size( ) );
}

int Pine::Components::GetComponentCount( EComponentType type )
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

	return FindAvailableDataSlot( comp );
}

Pine::IComponent* Pine::Components::GetComponent( EComponentType type, int index )
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

	return reinterpret_cast<IComponent*>( reinterpret_cast<std::uintptr_t>( comp->m_Data ) + ( comp->m_ComponentSize * index ) );
}

const char* Pine::Components::GetComponentTypeName( EComponentType type ) {
	return g_Components[ static_cast<int>( type ) ].m_Name;
}

Pine::IComponent* Pine::Components::CreateComponent( EComponentType type, bool standalone ) {
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

	if ( standalone )
	{
		const auto component = static_cast<IComponent*>( malloc( comp->m_ComponentSize ) );

		memcpy_s( component, comp->m_ComponentSize, comp->m_Component, comp->m_ComponentSize );

		return component;
	}

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

	componentPtr->SetStandalone( false );

	Log::Debug( "Pine::Components::CreateComponent( " + std::string( g_Components[ static_cast<int>( type ) ].m_Name ) + ", " + std::to_string( standalone ) + " ): slot -> " + std::to_string( slot ) );

	return componentPtr;
}

bool Pine::Components::DeleteComponent( IComponent* inputComponent )
{
	// If this is during shutdown, we might have called Pine::Components::Dispose( ) already.
	if ( g_Components.empty( ) )
	{
		return true;
	}

	Log::Debug( "Pine::Components::DeleteComponent( ): standalone -> " + std::to_string( inputComponent->GetStandalone( ) ) );

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

Pine::IComponent* Pine::Components::CopyComponent( const Pine::IComponent* inputComponent, bool standalone )
{
	Log::Debug( "Pine::Components::CopyComponent( ): standalone -> " + std::to_string( standalone ) );

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
		component = static_cast<IComponent*>( malloc( comp->m_ComponentSize ) );
	}
	else
	{
		const int slot = FindAvailableDataSlot( comp );

		if ( slot == -1 ) // TODO: Resize vector or something
		{
			Pine::Log::Warning( "Failed to find available slot for component!" );
			return nullptr;
		}

		// How do you do this properly?
		component = reinterpret_cast<IComponent*>( reinterpret_cast<std::uintptr_t>( comp->m_Data ) + ( comp->m_ComponentSize * slot ) );

		comp->m_DataValid[ slot ] = true;
	}

	memcpy_s( component, comp->m_ComponentSize, inputComponent, comp->m_ComponentSize );

	component->SetStandalone( standalone );

	return component;
}

void Pine::Components::Internal::RegisterComponent( IComponent* component, const size_t componentSize, const char* str ) {
	Component_t comp;

	comp.m_Component = component;
	comp.m_Name = str;
	comp.m_ComponentSize = componentSize;

	if ( component )
	{
		component->SetStandalone( false );

		ResizeData( &comp, componentSize, 128 ); // By default make space for 128 components
	}

	g_Components.push_back( comp );
}

void Pine::Components::Internal::ShowDebugUI( )
{
	static int selectedComponent = 0;
	static int newSize = 128;

	if ( ImGui::Begin( "Components System Debug", nullptr, 0 ) )
	{
		std::vector<const char*> components;

		components.reserve( g_Components.size( ) );

		for ( auto& c : g_Components )
			components.push_back( c.m_Name );

		ImGui::Combo( "Component Type", &selectedComponent, components.data( ), components.size( ) );

		auto component = &g_Components[ selectedComponent ];

		ImGui::Text( "Available slot: %d", FindAvailableDataSlot( component ) );
		ImGui::Text( "Size: %u elements", component->m_DataValidSize );
		ImGui::Text( "Allocated memory: %u bytes (%u KB)", component->m_DataSize + component->m_DataValidSize, ( component->m_DataSize + component->m_DataValidSize ) / 1000 );


		ImGui::InputInt( "##Size", &newSize, 2, 8, 0 );

		ImGui::SameLine( );

		if ( ImGui::Button( "Resize" ) )
		{
			ResizeData( component, component->m_ComponentSize, newSize );
		}
	}
	ImGui::End( );


}
