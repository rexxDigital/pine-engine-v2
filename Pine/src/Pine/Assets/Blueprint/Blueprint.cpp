#include "Blueprint.hpp"
#include "../../Entity/Entity.hpp"

void Pine::Blueprint::SaveEntity( Pine::Entity* target, Pine::Entity* entity )
{
	target->SetName( entity->GetName( ) );
	target->SetActive( entity->GetActive( ) );

	// Copy components
	for ( auto component : entity->GetComponents(  ) )
	{
		
	}
	
	// Copy children
	for ( auto child : entity->GetChildren( ) )
	{
		SaveEntity( target, child );
	}
}

void Pine::Blueprint::CreateFromEntity( Pine::Entity* entity )
{
	if ( m_Entity )
	{
		delete m_Entity;
	}

	m_Entity = new Entity( 0 );

	SaveEntity( m_Entity, entity );
}

void Pine::Blueprint::SpawnEntity( ) const
{
	if (!HasValidEntity(  ) )
	{
		return;
	}

	
 }

bool Pine::Blueprint::HasValidEntity( ) const
{
	return m_Entity != nullptr;
}

bool Pine::Blueprint::LoadFromFile( )
{
	return false;
}

bool Pine::Blueprint::SaveToFile( )
{
	return false;
}

void Pine::Blueprint::Dispose( )
{
	
}
