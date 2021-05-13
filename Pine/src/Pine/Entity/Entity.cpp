#include "Entity.hpp"

#include "../Entitylist/EntityList.hpp"

Pine::Entity::Entity( uint64_t id )
{
	m_Id = id;
	AddComponent( new Transform( ) );
}

Pine::Entity::Entity( uint64_t id, bool empty )
{
	m_Id = id;

	if ( !empty )
	{
		AddComponent( new Transform( ) );
	}
}

Pine::Entity::~Entity( )
{
	for ( auto component : m_Components ) {
		component->OnDestroy( );
		delete component;
	}

	// TODO: How are we going to handle this? Remove children?
	for ( auto child : m_Children )
	{
		child->SetParent( nullptr );
	}

	if ( m_Parent != nullptr )
	{
		m_Parent->RemoveChild( this );
	}
}

bool Pine::Entity::GetActive( ) const {
	return m_Active;
}

void Pine::Entity::SetActive( bool value ) {
	m_Active = value;
}

bool Pine::Entity::IsTemporary( ) const
{
	return m_IsTemporary;
}

void Pine::Entity::SetTemporary( bool value )
{
	m_IsTemporary = value;
}

const std::string& Pine::Entity::GetName( ) const {
	return m_Name;
}

void Pine::Entity::SetName( const std::string& str ) {
	m_Name = str;
}

uint64_t Pine::Entity::GetId( ) const {
	return m_Id;
}

uint64_t Pine::Entity::GetEntityIndex( ) const {
	return m_EntityIndex;
}

void Pine::Entity::SetEntityIndex( uint64_t indx ) {
	m_EntityIndex = indx;
}

Pine::Transform* Pine::Entity::GetTransform( ) const {
	return reinterpret_cast< Transform* >( m_Components[ 0 ] );
}

const std::vector<Pine::IComponent*>& Pine::Entity::GetComponents( ) const {
	return m_Components;
}

const std::vector<Pine::Entity*>& Pine::Entity::GetChildren( ) const
{
	return m_Children;
}

void Pine::Entity::SetParent( Entity* parent )
{
	m_Parent = parent;
}

Pine::Entity* Pine::Entity::GetParent( ) const
{
	return m_Parent;
}

Pine::Entity* Pine::Entity::CreateChild( bool createEntity )
{
	Pine::Entity* entity = nullptr;

	if ( createEntity )
		entity = Pine::EntityList::CreateEntity( );
	else
		entity = new Pine::Entity( 0, true );

	entity->SetParent( this );

	m_Children.push_back( entity );

	return entity;
}

void Pine::Entity::AddChild( Entity* entity )
{
	entity->SetParent( this );
	
	m_Children.push_back( entity );
}

void Pine::Entity::RemoveChild( Entity* entity )
{
	for ( int i = 0; i < m_Children.size( ); i++ )
	{
		if ( entity == m_Children[ i ] )
		{
			m_Children.erase( m_Children.begin( ) + i );
			break;
		}
	}

	entity->SetParent( nullptr );
}

void Pine::Entity::DeleteChildren( )
{
	for ( auto entity : m_Children )
	{
		EntityList::DeleteEntity( entity );
	}

	m_Children.clear( );
}

void Pine::Entity::AddComponent( IComponent* component ) {
	component->SetParent( this );
	component->OnCreated( );

	m_Components.push_back( component );
}

bool Pine::Entity::RemoveComponent( IComponent* component )
{
	int i = 0;
	for ( auto comp : m_Components )
	{
		if ( comp == component )
		{
			m_Components.erase( m_Components.begin( ) + i );
			return true;
		}

		i++;
	}

	return false;
}

bool Pine::Entity::RemoveComponent( const int index )
{
	if ( 0 > index || index >= m_Components.size( ) )
	{
		return false;
	}

	m_Components.erase( m_Components.begin( ) + index );

	return true;
}

void Pine::Entity::ClearComponents( )
{
	m_Components.clear( );
}
