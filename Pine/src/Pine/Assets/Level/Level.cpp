#include "Level.hpp"

#include "../../Entitylist/EntityList.hpp"

#include <fstream>


#include "../Assets.hpp"
#include "../../Core/Log/Log.hpp"
#include "../../Core/Serialization/Serialization.hpp"
#include "../../Rendering/RenderManager/RenderManager.hpp"
#include "../../Rendering/Skybox/Skybox.hpp"
#include "../Texture3D/Texture3D.hpp"

void Pine::Level::DisposeBlueprints( )
{
	for ( auto bp : m_Blueprints )
	{
		bp->Dispose( );
		delete bp;
	}

	m_Blueprints.clear( );
}

Pine::Blueprint* Pine::Level::CreateBlueprintOfEntity( Pine::Entity* entity )
{
	if ( !entity )
	{
		return nullptr;
	}

	auto blueprint = new Pine::Blueprint( );

	blueprint->CreateFromEntity( entity );

	return blueprint;
}

Pine::Level::Level( )
{
	m_Type = EAssetType::Level;
	m_LevelSettings = new Pine::LevelSettings;
}

int Pine::Level::GetBlueprintCount( ) const
{
	return m_Blueprints.size( );
}

void Pine::Level::CreateFromCurrentLevel( )
{
	auto& entities = Pine::EntityList::GetEntities( );

	uint64_t currentId = 1;
	Entity* currentCameraEntity = nullptr;

	if ( RenderManager::GetRenderingContext( )->m_Camera != nullptr )
		currentCameraEntity = RenderManager::GetRenderingContext( )->m_Camera->GetParent( );

	// Clear current list if we have one
	DisposeBlueprints( );

	// Create a blueprint for each entity
	for ( auto entity : entities )
	{
		// Ignore temporary entities such as editor entities.
		if ( entity->IsTemporary( ) )
		{
			continue;
		}

		if ( entity->GetParent(  ) != nullptr )
		{
			continue;
		}

		if ( currentCameraEntity == entity )
		{
			m_LevelSettings->m_CameraEntity = currentId;
		}

		m_Blueprints.push_back( CreateBlueprintOfEntity( entity ) );

		currentId++;
	}

	m_LevelSettings->m_Skybox = Pine::Skybox::GetSkyboxCubemap( );
}

void Pine::Level::Load( )
{
	uint64_t currentId = 1;

	// Clear current non temporaries entities
	Pine::EntityList::ClearEntities( );

	// Add our blueprint entities
	for ( auto bp : m_Blueprints )
	{
		auto entity = bp->SpawnEntity( );

		if ( m_LevelSettings->m_CameraEntity != 0 && m_LevelSettings->m_CameraEntity == currentId )
		{
			RenderManager::GetRenderingContext( )->m_Camera = entity->GetComponent<Camera>( );
		}
		
		currentId++;
	}

	if ( m_LevelSettings->m_Skybox != nullptr )
		Pine::Skybox::SetSkyboxCubemap( m_LevelSettings->m_Skybox );
}

Pine::LevelSettings* Pine::Level::GetSettings( ) const
{
	return m_LevelSettings;
}

bool Pine::Level::LoadFromFile( )
{
	if ( !std::filesystem::exists( m_FilePath ) )
	{
		return false;
	}

	// Since it's on the heap, don't forgot to clean up after ourselves!
	auto j = nlohmann::json( );

	std::ifstream stream( m_FilePath );

	// Hopefully this works with large files.
	stream >> j;

	stream.close( );

	try {
		DisposeBlueprints( );

		// Loop through each json object at entities
		for ( auto entJson : j[ "entities" ] )
		{
			// Attempt to load a blueprint.
			auto blueprint = new Pine::Blueprint( );

			blueprint->FromJson( entJson );

			m_Blueprints.push_back( blueprint );
		}

		if ( j.contains( "activeCamera" ) )
		{
			m_LevelSettings->m_CameraEntity = j[ "activeCamera" ].get<std::uint64_t>( );
		}

		if ( j.contains( "skybox" ) )
		{
			m_LevelSettings->m_Skybox = Pine::Assets::GetAsset<Texture3D>( j[ "skybox" ] );
		}
	}
	catch ( std::exception& e ) {
		Pine::Log::Error( "JSON parsing error: " + std::string( e.what( ) ) );

		return false;
	}

	return true;
}

bool Pine::Level::SaveToFile( )
{
	auto j = nlohmann::json( );

	for ( auto bp : m_Blueprints )
	{
		// Something probably went wrong here, since they should always contain an entity.
		if ( !bp->HasValidEntity( ) )
		{
			continue;
		}

		// Insert an entities array at "entities", since we might store more level data in the future.
		j[ "entities" ].push_back( bp->ToJson( ) );
	}

	j[ "activeCamera" ] = m_LevelSettings->m_CameraEntity;

	if ( m_LevelSettings->m_Skybox )
		j[ "skybox" ] = m_LevelSettings->m_Skybox->GetPath( ).string( );

	std::ofstream stream( m_FilePath );

	stream << j;

	stream.close( );

	return true;
}

void Pine::Level::Dispose( )
{
	delete m_LevelSettings;

	DisposeBlueprints( );
}
