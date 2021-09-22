#include "Components.hpp"

#include "Behavior/Behavior.hpp"
#include "TerrainRenderer/TerrainRenderer.hpp"

namespace {

	struct Component_t
	{
		Pine::IComponent* m_Component;
		std::string m_Name;
		void* m_Data = nullptr;
		int m_DataSize = 0;
		int m_DataPointer = 0;
	};

	std::vector<Pine::IComponent*> g_Components;
	std::vector<std::string> g_ComponentNames;
}

int Pine::Components::GetComponentCount( ) {
	return g_Components.size( );
}

const char* Pine::Components::GetComponentName( EComponentType type ) {
	return g_ComponentNames[ static_cast< int >( type ) ].c_str( );
}

Pine::IComponent* Pine::Components::CreateComponent( EComponentType type ) {
	for ( auto component : g_Components ) {
		if ( !component )
			continue;

		if ( component->GetType( ) == type ) {
			return component->Clone( );
		}
	}

	return nullptr;
}

void Pine::Components::Internal::RegisterPineComponents( ) {
	RegisterComponent( nullptr, "Invalid" ); 
	RegisterComponent( new Transform( ), "Transform" );
	RegisterComponent( new ModelRenderer( ), "ModelRenderer" );
	RegisterComponent( new Camera( ), "Camera" );
	RegisterComponent( new Light( ), "Light" );
	RegisterComponent( nullptr, "Native Script" );
	RegisterComponent( new Behavior( ), "Behavior" );
	RegisterComponent( new TerrainRenderer( ), "Terrain Renderer" );
}

void Pine::Components::Internal::RegisterComponent( IComponent* component, const std::string& str ) {
	g_Components.push_back( component );
	g_ComponentNames.push_back( str );
}
