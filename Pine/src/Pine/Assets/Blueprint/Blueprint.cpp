#include "Blueprint.hpp"

#include "../../Core/Log/Log.hpp"
#include "../../Core/Serialization/Serialization.hpp"
#include "../../Entity/Entity.hpp"
#include "../../Entitylist/EntityList.hpp"

#include "../../Components/Camera/Camera.hpp"
#include "../../Components/Light/Light.hpp"
#include "../../Components/ModelRenderer/ModelRenderer.hpp"
#include "../../Components/TerrainRenderer/TerrainRenderer.hpp"
#include "../../Components/Components.hpp"

#include <fstream>

namespace
{
	Pine::IComponent* CreateComponentFromType( Pine::ComponentType type )
	{
		return nullptr;
	}

	void SaveEntityJson( nlohmann::json& j, Pine::Entity* e )
	{
		j[ "name" ] = e->GetName( );
		j[ "active" ] = e->GetActive( );
		j[ "static" ] = e->GetStatic( );

		// Save components
		int componentId = 0;
		for ( auto component : e->GetComponents( ) )
		{
			nlohmann::json componentJson;

			componentJson[ "type" ] = static_cast< int >( component->GetType( ) );

			component->SaveToJson( componentJson );

			j[ "components" ].push_back( componentJson );

			componentId++;
		}

		// Save children
		int childrenId = 0;
		for ( auto child : e->GetChildren( ) )
		{
			nlohmann::json childJson;

			SaveEntityJson( childJson, child );

			j[ "children" ].push_back( childJson );

			childrenId++;
		}
	}

	void LoadEntityJson( Pine::Entity* e, const nlohmann::json& j )
	{
		e->SetName( j[ "name" ] );
		e->SetActive( j[ "active" ] );

        if ( j.contains( "static" ) )
		    e->SetStatic( j[ "static" ] );

		// Load components
		for ( auto componentJson : j[ "components" ] )
		{
			const auto component = Pine::Components->CreateComponent( static_cast< Pine::ComponentType >( componentJson[ "type" ] ), true );
			if ( component == nullptr )
			{
				Pine::Log->Warning( "PARSING: Failed to create component of type " + componentJson[ "type" ] );
				continue;
			}

			component->LoadFromJson( componentJson );

			e->AddComponent( component );
		}

		// Load children
		if ( j.contains( "children" ) )
		{
			for ( const auto& childJson : j.at( "children" ) )
			{
				const auto child = e->CreateChild( false );

				LoadEntityJson( child, childJson );
			}
		}
	}

}

void Pine::Blueprint::CopyEntity( Pine::Entity* target, const Pine::Entity* entity, bool createInstance ) const
{
	target->SetName( entity->GetName( ) );
	target->SetActive( entity->GetActive( ) );
	target->SetStatic( entity->GetStatic( ) );

	// Copy components
	for ( const auto component : entity->GetComponents( ) )
	{
		target->AddComponent( Components->CopyComponent( component, target, !createInstance ) );
	}

	// Copy children
	for ( const auto child : entity->GetChildren( ) )
	{
		const auto newChild = target->CreateChild( createInstance );

		newChild->ClearComponents( );

		CopyEntity( newChild, child, createInstance );
	}
}

Pine::Blueprint::Blueprint( )
{
	m_Type = AssetType::Blueprint;
}

void Pine::Blueprint::CreateFromEntity( const Pine::Entity* entity )
{
	delete m_Entity;

	m_Entity = new Entity( 0, true );

	CopyEntity( m_Entity, entity, false );
}

Pine::Entity* Pine::Blueprint::SpawnEntity( ) const
{
	if ( !HasValidEntity( ) )
	{
		return nullptr;
	}

	Entity* newEntity = Pine::EntityList->CreateEntity( );

	newEntity->ClearComponents( );

	CopyEntity( newEntity, m_Entity, true );

	return newEntity;
}

bool Pine::Blueprint::HasValidEntity( ) const
{
	return m_Entity != nullptr;
}

Pine::Entity* Pine::Blueprint::GetInternalEntity( ) const
{
	return m_Entity;
}

nlohmann::json Pine::Blueprint::ToJson( ) const
{
	nlohmann::json j;

	SaveEntityJson( j, m_Entity );

	return j;
}

bool Pine::Blueprint::FromJson( nlohmann::json& j )
{
	delete m_Entity;

	m_Entity = new Entity( 0, true );

	LoadEntityJson( m_Entity, j );

	return true;
}

bool Pine::Blueprint::LoadFromFile( )
{
	const auto j = Pine::Serialization::LoadJSONFromFile( m_FilePath.string( ) );

	try
	{

		delete m_Entity;

		m_Entity = new Entity( 0, true );

		LoadEntityJson( m_Entity, j );

	}
	catch ( std::exception& e )
	{
		Pine::Log->Error( "JSON parsing error: " + std::string( e.what( ) ) );
		return false;
	}

	return true;
}

bool Pine::Blueprint::SaveToFile( )
{
	if ( m_Entity == nullptr )
	{
		return false;
	}

	nlohmann::json j;

	SaveEntityJson( j, m_Entity );

	std::ofstream stream( m_FilePath );

	stream << j;

	stream.close( );

	return true;
}

void Pine::Blueprint::Dispose( )
{
	delete m_Entity;
}
