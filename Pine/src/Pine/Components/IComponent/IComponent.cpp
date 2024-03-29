#include "IComponent.hpp"

Pine::ComponentType Pine::IComponent::GetType( ) const
{
	return m_ComponentType;
}

void Pine::IComponent::SetParent( Pine::Entity* parent )
{
	m_Parent = parent;
}

Pine::Entity* Pine::IComponent::GetParent( ) const
{
	return m_Parent;
}

bool Pine::IComponent::GetActive( ) const
{
	return m_Active;
}

void Pine::IComponent::SetActive( bool value )
{
	m_Active = value;
}

bool Pine::IComponent::GetStandalone( ) const
{
	return m_Standalone;
}

void Pine::IComponent::SetStandalone( bool value )
{
	m_Standalone = value;
}

void Pine::IComponent::OnRender( )
{

}

void Pine::IComponent::OnDestroyed( )
{

}

void Pine::IComponent::OnCreated( )
{

}

void Pine::IComponent::OnCopied( const IComponent* old )
{

}

void Pine::IComponent::LoadFromJson( nlohmann::json& j )
{

}

void Pine::IComponent::SaveToJson( nlohmann::json& j )
{

}

void Pine::IComponent::OnRenderUI() {

}
