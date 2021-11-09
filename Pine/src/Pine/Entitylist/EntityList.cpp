#pragma once
#include "EntityList.hpp"

namespace Pine
{

	class CEntityList : public IEntityList
	{
	private:
		// https://stackoverflow.com/questions/45447361/how-to-move-certain-elements-of-stdvector-to-a-new-index-within-the-vector
		template <typename t> void move( std::vector<t>& v, size_t oldIndex, size_t newIndex ) {
			if ( oldIndex > newIndex )
				std::rotate( v.rend( ) - oldIndex - 1, v.rend( ) - oldIndex, v.rend( ) - newIndex );
			else
				std::rotate( v.begin( ) + oldIndex, v.begin( ) + oldIndex + 1, v.begin( ) + newIndex + 1 );
		}

		std::vector<Entity> m_Entities;
		uint64_t m_CurrentId = 0;
	public:

		void Setup( ) override
		{
			m_Entities.reserve( 128 );
		}

		Entity* CreateEntity( ) override
		{
			m_Entities.emplace_back( m_CurrentId++ );

			const auto ptr = &m_Entities[ m_Entities.size( ) - 1 ];

			ptr->SetEntityIndex( m_Entities.size( ) - 1 );
			ptr->GetTransform( )->SetParent( ptr );

			return ptr;
		}

		Entity* CreateEntity( const std::string& str ) override
		{
			auto entity = Pine::Entity( m_CurrentId++ );

			entity.SetEntityIndex( m_Entities.size( ) );
			entity.SetName( str );

			m_Entities.push_back( entity );

			const auto ptr = &m_Entities[ m_Entities.size( ) - 1 ];

			ptr->GetTransform( )->SetParent( ptr );

			return ptr;
		}

		bool DeleteEntity( Entity* entity ) override
		{
			for ( int i = 0; i < m_Entities.size( ); i++ ) {
				if ( &m_Entities[ i ] == entity ) {
					m_Entities.erase( m_Entities.begin( ) + i );

					return true;
				}
			}

			return false;
		}

		// This should be changed soon, moving entities is not efficient and is only done for the GUI.
		void MoveEntity( Entity* entity, int newPosition ) override
		{
			int entityVectorIndex = -1;

			for ( int i = 0; i < m_Entities.size( ); i++ )
			{
				if ( &m_Entities[ i ] == entity )
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

		const std::vector<Pine::Entity>& GetEntities( ) override
		{
			return m_Entities;
		}

		Entity* GetEntity( const int index ) override
		{
			return &m_Entities[ index ];
		}

		void ClearEntities( bool temp ) override
		{
			for ( int i = 0; i < m_Entities.size( ); i++ )
			{
				if ( m_Entities[ i ].IsTemporary( ) )
					continue;

				auto entityPtr = m_Entities[ i ];

				m_Entities.erase( m_Entities.begin( ) + i );

				i--;
			}
		}

		void RunOnSetup( ) override
		{
			for ( auto& entity : m_Entities ) {
				for ( const auto comp : entity.GetComponents( ) ) {
					comp->OnSetup( );
				}
			}
		}

		void RunOnUpdate( float deltaTime ) override
		{
			for ( auto& entity : m_Entities ) {
				for ( const auto comp : entity.GetComponents( ) ) {
					comp->OnUpdate( deltaTime );
				}
			}
		}

	};

	IEntityList* CreateEntityListInterface( )
	{
		return new CEntityList;
	}

}
