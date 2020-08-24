#include "Entity.hpp"

Pine::Entity::Entity(uint64_t id) 
{
	m_Id = id;
	m_Components.push_back(new Transform());
}

Pine::Entity::~Entity()
{
	for (auto component : m_Components) {
		component->OnDestroy();
		delete component;
	}
}

bool Pine::Entity::GetActive() const {
	return m_Active;
}

void Pine::Entity::SetActive(bool value) {
	m_Active = value;
}

const std::string& Pine::Entity::GetName() const {
	return m_Name;
}

void Pine::Entity::SetName(const std::string& str) {
	m_Name = str;
}

uint64_t Pine::Entity::GetId() const {
	return m_Id;
}

uint64_t Pine::Entity::GetEntityIndex() const {
	return m_EntityIndex;
}

uint64_t& Pine::Entity::GetEntityFlags() {
	return m_EntityFlags;
}

void Pine::Entity::SetEntityIndex(uint64_t indx) {
	m_EntityIndex = indx;
}

Pine::Transform* Pine::Entity::GetTransform() const {
	return reinterpret_cast<Transform*>(m_Components[0]);
}

const std::vector<Pine::IComponent*>& Pine::Entity::GetComponents() const {
	return m_Components;
}

void Pine::Entity::AddComponent(IComponent* component) {
	component->SetParent(this);

	m_Components.push_back(component);
}