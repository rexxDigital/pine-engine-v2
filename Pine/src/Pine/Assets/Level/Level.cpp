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

Pine::Blueprint* Pine::Level::CreateBlueprintOfEntity( const Pine::Entity* entity )
{
	if ( !entity )
	{
		return nullptr;
	}

	const auto blueprint = new Pine::Blueprint( );

	blueprint->CreateFromEntity( entity );

	return blueprint;
}

Pine::Level::Level( )
{
	m_Type = AssetType::Level;
	m_LevelSettings = new Pine::LevelSettings;
}

int Pine::Level::GetBlueprintCount( ) const
{
	return m_Blueprints.size( );
}

void Pine::Level::CreateFromCurrentLevel( )
{
	auto& entities = Pine::EntityList->GetEntities( );

    // Write camera index
	if ( RenderManager->GetRenderingContext( )->m_Camera != nullptr )
    {
        auto camEntity = RenderManager->GetRenderingContext( )->m_Camera->GetParent( );

        int id = 1;
        for (auto entity : EntityList->GetEntities())
        {
            if (entity->IsTemporary())
                continue;

            if (entity == camEntity)
            {
                m_LevelSettings->m_CameraEntity = id;
                break;
            }

            id++;
        }
    }

	// Clear current list if we have one
	DisposeBlueprints( );

	// Create a blueprint for each entity
	for ( auto entity : entities )
	{
		// Ignore temporary entities such as editor entities.
		if ( entity->IsTemporary( ) )
			continue;

		// Ignore children as we write them separately later.
		if ( entity->GetParent( ) != nullptr )
            continue;

        m_Blueprints.push_back( CreateBlueprintOfEntity( entity ) );
    }

	m_LevelSettings->m_Skybox = Pine::Skybox->GetSkyboxCubemap( );
}

void Pine::Level::Load( )
{
    // Clear current non temporaries entities
	Pine::EntityList->ClearEntities( );

    const int levelEntityOffset = Pine::EntityList->GetEntities().size();

	// Add our blueprint entities
	for ( auto bp : m_Blueprints )
	{
		bp->SpawnEntity( );
	}

    // Apply camera
    if (m_LevelSettings->m_CameraEntity != 0)
    {
        int entityIndex = (m_LevelSettings->m_CameraEntity - 1) + levelEntityOffset;

        auto entity = Pine::EntityList->GetEntities()[entityIndex];
        if (entity)
        {
            auto camera = entity->GetComponent<Camera>();

            if (camera)
                RenderManager->GetRenderingContext()->m_Camera = camera;
        }
    }

	if ( m_LevelSettings->m_Skybox != nullptr )
		Pine::Skybox->SetSkyboxCubemap( m_LevelSettings->m_Skybox );
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

	auto j = nlohmann::json( );

	std::ifstream stream( m_FilePath );

	// Hopefully this is fast enough with large files...
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
			m_LevelSettings->m_Skybox = dynamic_cast< Texture3D* >( Pine::Assets->LoadFromFile( j[ "skybox" ] ) );
		}
	}
	catch ( std::exception& e ) {
		Pine::Log->Error( "JSON parsing error: " + std::string( e.what( ) ) );

		return false;
	}

	return true;
}

bool Pine::Level::SaveToFile( )
{
	auto j = nlohmann::json( );

	for ( const auto bp : m_Blueprints )
	{
		// Something probably went wrong here, since they should always contain an entity.
		if ( !bp->HasValidEntity( ) )
		{
			continue;
		}

		// Insert an entities array at "entities".
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
