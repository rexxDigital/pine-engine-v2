#pragma once
#include "EntityList.hpp"

namespace Pine
{

    // The initial amount of entity slots available.
    constexpr int NrEntitySlots = 1024;

	class CEntityList : public IEntityList
	{
	private:
        // Incremental number throughout the entire application, so each entity will always have a unique
        // identifier.
        uint64_t m_CurrentId = 0;

        // Array of entities with the size of NrEntitySlots
        Entity* m_Entities = nullptr;

        // Array of booleans to determine if an element within the array is used or not
        bool* m_EntitySlots = nullptr;

        // Will always have the highest entity index, so enumerate through this instead.
        int m_CurrentHighestEntityIndex = 0;

        // A read-only mirror of the entities
        std::vector<Entity*> m_EntitiesVec;

        int FindAvailableEntitySlot()
        {
            for (int i = 0; i < NrEntitySlots;i++)
            {
                if (!m_EntitySlots[i])
                {
                    return i;
                }
            }

            return -1;
        }
	public:

		void Setup( ) override
		{
            m_Entities = static_cast< Entity* >( malloc( sizeof( Entity ) * NrEntitySlots ) );
		    m_EntitySlots = new bool[NrEntitySlots];

            memset(m_Entities, 0, sizeof( Entity ) * NrEntitySlots);
            memset(m_EntitySlots, 0, sizeof(bool) * NrEntitySlots);
        }

		Entity* CreateEntity( ) override
		{
            int entitySlot = FindAvailableEntitySlot();

            if (entitySlot == -1)
            {
                return nullptr;
            }

            void* entityPtr = &m_Entities[entitySlot];

            memset(entityPtr, 0, sizeof(Entity));

            Entity* entity = new(entityPtr)Entity(m_CurrentId++);

            entity->SetEntityIndex( entitySlot );
            entity->GetTransform( )->SetParent( entity );

            m_EntitySlots[entitySlot] = true;

            m_EntitiesVec.push_back(entity);

			return entity;
		}

		Entity* CreateEntity( const std::string& str ) override
		{
			auto entity = CreateEntity();

            entity->SetName(str);

			return entity;
		}

		bool DeleteEntity( Entity* entity ) override
		{
            const int slot = entity->GetEntityIndex();

            entity->~Entity();

            // No need to clear the memory at m_Entities, since CreateEntity() already does that.
            // We'll simply just mark the slot as available instead.

            m_EntitySlots[slot] = false;

            for (int i = 0; i < m_EntitiesVec.size();i++)
            {
                if (m_EntitiesVec[i] == entity)
                {
                    m_EntitiesVec.erase(m_EntitiesVec.begin() + i);
                    break;
                }
            }

            return true;
        }

        int GetEntityCount() override
        {
            return m_CurrentHighestEntityIndex;
        }

		Entity* GetEntity( const int index ) override
		{
			return &m_Entities[ index ];
		}

		Entity* FindEntity( const std::string& name ) override
		{
			for ( auto entity : m_EntitiesVec )
			{
				if ( entity->GetName( ) == name )
				{
					return entity;
				}
			}

			return nullptr;
		}

		void ClearEntities( bool temp ) override
		{
			for ( int i = 0; i < m_EntitiesVec.size( ); i++ )
			{
				if ( m_EntitiesVec[ i ]->IsTemporary( ) )
					continue;

				auto entityPtr = m_EntitiesVec[ i ];

                DeleteEntity(entityPtr);

                i--;
			}
		}

		void RunOnSetup( ) override
		{
			for ( auto entity : m_EntitiesVec )
			{
				for ( const auto comp : entity->GetComponents( ) )
				{
					comp->OnSetup( );
				}
			}
		}

		void RunOnUpdate( float deltaTime ) override
		{
			for ( auto entity : m_EntitiesVec )
			{
				for ( const auto comp : entity->GetComponents( ) )
				{
					comp->OnUpdate( deltaTime );
				}
			}
		}

        const std::vector<Entity*>& GetEntities( ) override
        {
            return m_EntitiesVec;
        }

        void Update( ) override
        {
            int highestIndex = 0;

            for (int i = 0; i < NrEntitySlots;i++)
            {
                if (m_EntitySlots[i])
                {
                    highestIndex = i;
                }
            }

            m_CurrentHighestEntityIndex = highestIndex;
        }

	};

	IEntityList* CreateEntityListInterface( )
	{
		return new CEntityList;
	}

}
