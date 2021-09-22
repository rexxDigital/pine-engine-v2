#include "IComponent.hpp"

Pine::EComponentType Pine::IComponent::GetType( ) const
{
	return m_ComponentType;
}

void Pine::IComponent::SetParent( Pine::Entity* parent ) {
	m_Parent = parent;
}

Pine::Entity* Pine::IComponent::GetParent( ) {
	return m_Parent;
}

const bool Pine::IComponent::GetActive( ) const {
	return m_Active;
}

void Pine::IComponent::SetActive( bool value ) {
	m_Active = value;
}

void Pine::IComponent::OnRender( ) {

}

void Pine::IComponent::OnDestroy( ) {

}

void Pine::IComponent::OnCreated( )
{

}

void Pine::IComponent::LoadFromJson( nlohmann::json& j )
{

}

void Pine::IComponent::SaveToJson( nlohmann::json& j )
{

}
