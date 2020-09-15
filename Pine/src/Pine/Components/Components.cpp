#include "Components.hpp"

namespace
{
	std::vector<Pine::IComponent*> g_Components;
}

int Pine::Components::GetComponentCount( )
{
	return g_Components.size( );
}

const char* Pine::Components::GetComponentName( EComponentType type )
{
	return Pine::SComponentNames[ static_cast< int >( type ) ];
}

Pine::IComponent* Pine::Components::CreateComponent( EComponentType type )
{
	for ( auto component : g_Components )
	{
		if ( component->GetType(  ) == type )
		{
			return component->Clone( );
		}
	}

	return nullptr;
}

void Pine::Components::Internal::RegisterPineComponents( )
{
	RegisterComponent( new Transform( ) );
	RegisterComponent( new ModelRenderer( ) );
	RegisterComponent( new Camera( ) );
	RegisterComponent( new Light( ) );
}

void Pine::Components::Internal::RegisterComponent( IComponent* component )
{
	g_Components.push_back( component );
}
