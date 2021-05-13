#pragma once
#include "EntityList.hpp"

namespace {

	// https://stackoverflow.com/questions/45447361/how-to-move-certain-elements-of-stdvector-to-a-new-index-within-the-vector
	template <typename t> void move( std::vector<t>& v, size_t oldIndex, size_t newIndex ) 	{
		if ( oldIndex > newIndex )
			std::rotate( v.rend( ) - oldIndex - 1, v.rend( ) - oldIndex, v.rend( ) - newIndex );
		else
			std::rotate( v.begin( ) + oldIndex, v.begin( ) + oldIndex + 1, v.begin( ) + newIndex + 1 );
	}
	
	std::vector<Pine::Entity*> m_Entities;
	uint64_t m_CurrentId = 0;

}

Pine::Entity* Pine::EntityList::CreateEntity( ) {
	auto entity = new Pine::Entity( m_CurrentId++ );

	entity->SetEntityIndex( m_Entities.size( ) );

	m_Entities.push_back( entity );

	return entity;
}

Pine::Entity* Pine::EntityList::CreateEntity( const std::string& str ) {
	auto entity = new Pine::Entity( m_CurrentId++ );

	entity->SetEntityIndex( m_Entities.size( ) );
	entity->SetName( str );

	m_Entities.push_back( entity );

	return entity;
}

bool Pine::EntityList::DeleteEntity( Entity* entity ) {
	for ( int i = 0; i < m_Entities.size( ); i++ ) {
		if ( m_Entities[ i ] == entity ) {
			m_Entities.erase( m_Entities.begin( ) + i );
			delete entity;
			return true;
		}
	}

	return false;
}

void Pine::EntityList::MoveEntity( Entity* entity, int newPosition )
{
	int entityVectorIndex = -1;

	for ( int i = 0; i < m_Entities.size(  );i++ )
	{
		if ( m_Entities[ i ] == entity )
		{
			entityVectorIndex = i;
			break;
		}
	}

	if ( entityVectorIndex == -1 )
	{
		return;
	}

	move( m_Entities, entityVectorIndex, newPosition );
}

const std::vector <Pine::Entity*>& Pine::EntityList::GetEntities( ) {
	return m_Entities;
}

void Pine::EntityList::ClearEntities( bool temp )
{
	for ( int i = 0; i < m_Entities.size( ); i++ )
	{
		if ( m_Entities[ i ]->IsTemporary( ) )
			continue;

		auto entityPtr = m_Entities[ i ];
		
		m_Entities.erase( m_Entities.begin( ) + i );

		delete entityPtr;
		
		i--;
	}
}

void Pine::EntityList::RunOnSetup( ) {
	for ( auto entity : m_Entities ) {
		for ( auto comp : entity->GetComponents( ) ) {
			comp->OnSetup( );
		}
	}
}

void Pine::EntityList::RunOnUpdate( float deltaTime )
{
	for ( auto entity : m_Entities ) {
		for ( auto comp : entity->GetComponents( ) ) {
			comp->OnUpdate( deltaTime );
		}
	}
}
