#pragma once
#include "EntityList.hpp"

namespace {

	std::vector<Pine::Entity*> m_Entities;
	uint64_t m_CurrentId = 0;

}

Pine::Entity* Pine::EntityList::CreateEntity() {
	auto entity = new Pine::Entity(m_CurrentId++);

	entity->SetEntityIndex(m_Entities.size());
	
	m_Entities.push_back(entity);

	return entity;
}

Pine::Entity* Pine::EntityList::CreateEntity(const std::string& str) {
	auto entity = new Pine::Entity(m_CurrentId++);

	entity->SetEntityIndex(m_Entities.size());
	entity->SetName(str),

	m_Entities.push_back(entity);

	return entity;
}

bool Pine::EntityList::DeleteEntity(Entity* entity) {
	for (int i = 0; i < m_Entities.size(); i++) {
		if (m_Entities[i] == entity) {
			m_Entities.erase(m_Entities.begin() + i);
			delete entity;
			return true;
		}
	}

	return false;
}

const std::vector <Pine::Entity*> & Pine::EntityList::GetEntities() {
	return m_Entities;
}

void Pine::EntityList::ClearEntities( bool temp )
{
	for ( int i = 0; i < m_Entities.size(  );i++)
	{
		if ( m_Entities[ i ]->IsTemporary(  ) )
			continue;

		m_Entities.erase( m_Entities.begin( ) + i );

		i--;
 	}
}

void Pine::EntityList::RunOnSetup() {
	for (auto entity : m_Entities) {
		for (auto comp : entity->GetComponents()) {
			comp->OnSetup();
		}
	}
}
