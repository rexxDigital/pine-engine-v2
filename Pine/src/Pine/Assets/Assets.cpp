#include "Assets.hpp"
#include "../Core/Core.hpp"
#include "../Core/Log/Log.hpp"

#include "Blueprint/Blueprint.hpp"
#include "Invalid/InvalidAsset.hpp"
#include "Level/Level.hpp"
#include "Material/Material.hpp"
#include "Model/Model.hpp"
#include "Script/Script.hpp"
#include "Terrain/Terrain.hpp"
#include "Texture2D/Texture2D.hpp"
#include "Texture3D/Texture3D.hpp"

namespace {
	std::unordered_map<std::string, Pine::IAsset*> m_Assets;

	// This is written to translate file extensions to loaded asset objects in memory.

	struct AssetFactory_t
	{
		AssetFactory_t( std::vector<std::string> fileExtensions, Pine::EAssetType type, std::function<Pine::IAsset*( )> factory )
		{
			m_FileExtensions = fileExtensions;
			m_Type = type;
			m_Factory = factory;
		}

		// The file extensions for this asset type.
		std::vector<std::string> m_FileExtensions;

		Pine::EAssetType m_Type;

		// This function should create the asset object itself.
		std::function<Pine::IAsset* ( )> m_Factory;
	};

	std::vector<AssetFactory_t> m_AssetFactories;

	void CreateAssetFactories( )
	{
		// Some asset types (like models, textures) could possible have more file extensions supported here, as the library that handles the parsing
		// supports more. I've listed the popular ones so far.

		m_AssetFactories.push_back( AssetFactory_t( { ".mat" }, Pine::EAssetType::Material, [ ]( ) { return new Pine::Material( ); } ) );
		m_AssetFactories.push_back( AssetFactory_t( { ".shr" }, Pine::EAssetType::Shader, [ ]( ) { return new Pine::Shader( ); } ) );
		m_AssetFactories.push_back( AssetFactory_t( { ".obj", ".fbx", ".3d", ".3ds", ".stl", ".dae", ".stp", ".wrl", ".ply" }, Pine::EAssetType::Model, [ ]( ) { return new Pine::Model( ); } ) );
		m_AssetFactories.push_back( AssetFactory_t( { ".png", ".jpg", ".jpeg", ".tga", ".bmp", ".psd", ".gif" }, Pine::EAssetType::Texture2D, [ ]( ) { return new Pine::Texture2D( ); } ) );
		m_AssetFactories.push_back( AssetFactory_t( { ".cmap" }, Pine::EAssetType::Texture3D, [ ]( ) { return new Pine::Texture3D( ); } ) );
		m_AssetFactories.push_back( AssetFactory_t( { ".bpt" }, Pine::EAssetType::Blueprint, [ ]( ) { return new Pine::Blueprint( ); } ) );
		m_AssetFactories.push_back( AssetFactory_t( { ".lvl" }, Pine::EAssetType::Level, [ ]( ) { return new Pine::Level( ); } ) );
		m_AssetFactories.push_back( AssetFactory_t( { ".as" }, Pine::EAssetType::Script, [ ]( ) { return new Pine::Script( ); } ) );
		m_AssetFactories.push_back( AssetFactory_t( { ".ter" }, Pine::EAssetType::Terrain, [ ]( ) { return new Pine::Terrain( ); } ) );

		Pine::Log::Message( "Loaded " + std::to_string( m_AssetFactories.size( ) ) + " asset factories." );
	}

	AssetFactory_t* GetAssetFactoryFromFileName( const std::string& fileName )
	{
		// Find the correct factory
		for ( auto& factory : m_AssetFactories ) 
		{
			for ( auto& extension : factory.m_FileExtensions ) 
			{
				if ( Pine::String::EndsWith( fileName, extension ) )
				{
					return &factory;
				}
			}
		}

		// We couldn't find any.

		return nullptr;
	}
}

Pine::IAsset* Pine::Assets::LoadFromFile( const std::string& filePath, bool readOnly ) {
	// ".asset" files are files that some assets use to store additional data for the asset,
	// since some asset files have standards, such as model files.
	if ( Pine::String::EndsWith( filePath, ".asset" ) )
		return nullptr;

	if ( m_Assets.count( filePath ) > 0 ) 
	{
		auto asset = m_Assets[ filePath ];

		// Reload the file if it has been updated, do it a bit differently for shaders though.
		if ( asset->GetType( ) == EAssetType::Shader )
		{
			if ( auto shader = dynamic_cast< Pine::Shader* >( asset ) )
			{
				bool shaderFilesUpdated = false;

				for ( auto shaderFileAsset : shader->GetAttachedShaderFiles( ) )
				{
					if ( shaderFileAsset->HasBeenUpdated( ) )
					{
						shaderFilesUpdated = true;
						shaderFileAsset->UpdateLastWriteTime( );
					}
				}

				if ( shaderFilesUpdated )
				{
					Log::Message( "Reloading shader " + asset->GetFileName( ) + " since it's files has been updated." );

					shader->Dispose( );
					shader->LoadFromFile( );
				}
			}
		}

		// Reload the file if it has been updated.
		if ( !asset->IsMapped( ) && asset->HasBeenUpdated( ) ) 
		{
			Log::Message( "Reloading file " + asset->GetFileName( ) + " since it has been updated." );

			// We will have to dispose the old stuff.
			// I am not too sure right now this reload system will work, since some variables will be the same.
			// The solution might be just to remove the old object completely and treat it as a new object.
			asset->Dispose( );

			// Load the asset again
			if ( !asset->LoadFromFile( ) )
			{
				Log::Warning( "Failed to reload file, " + asset->GetFileName( ) );
				return nullptr;
			}

			// Mark it as reloaded
			asset->UpdateLastWriteTime( );
		}

		return asset;
	}

	const auto factory = GetAssetFactoryFromFileName( filePath );

	IAsset* asset;

	if ( factory != nullptr )
	{
		// Quick hack to prevent loading sky box images twice.
		if ( factory->m_Type == EAssetType::Texture2D && std::filesystem::exists( std::filesystem::path( filePath ).parent_path( ).string( ) + "\\IGNORE_TEXTURES" ) ) 
		{
			return nullptr;
		}

		asset = factory->m_Factory( );
	}
	else
	{
		asset = new InvalidAsset( );
	}

	asset->SetFilePath( filePath );

	Log::Debug( "Loading asset '" + filePath + "'..." );

	if ( !asset->LoadFromFile( ) ) 
	{
		return nullptr;
	}

	asset->SetReadOnly( readOnly );
	asset->UpdateLastWriteTime( );

	m_Assets[ filePath ] = asset;

	return asset;
}

int Pine::Assets::LoadFromDirectory( const std::string& directoryPath, bool readOnly )
{
	uint32_t loadedAssets = 0;

	if ( !std::filesystem::exists( directoryPath ) )
	{
		Log::Error( "Failed to load assets from, " + directoryPath + ", directory does not exist." );
		return 0;
	}
	
	for ( const auto& dirEntry : std::filesystem::recursive_directory_iterator( directoryPath ) ) 
	{
		if ( dirEntry.is_directory( ) )
			continue;

		if ( LoadFromFile( dirEntry.path( ).string( ), readOnly ) ) 
		{
			loadedAssets++;
		}
	}
	
	// We're going to assume something went wrong as it loaded 0 assets.
	if ( loadedAssets == 0 )
		Log::Warning( "Loaded 0 assets from the directory " + directoryPath );

	return loadedAssets;
}

Pine::IAsset* Pine::Assets::GetAsset( const std::string& assetPath )
{
	if ( m_Assets.count( assetPath ) == 0 )
		return nullptr;

	return m_Assets[ assetPath ];
}

void Pine::Assets::MapAsset( IAsset* asset, const std::string& fakePath )
{
	if ( Pine::String::EndsWith( fakePath, ".asset" ) )
	{
		// If you want to do this anyway, go ahead. Remove this code, I do not care.
		// While writing this code, I would want to avoid ever having to map a ".asset" file
		// as I want every asset loaded by the engine to have it's privacy when it comes to their
		// ".asset" file.
		
		Log::Error( "Failed to map '" + fakePath + "', cannot map a '.asset' file." );
		
		return;
	}
	
	asset->SetFilePath( fakePath );
	asset->SetReadOnly( true );
	asset->SetMapped( true );

	m_Assets[ fakePath ] = asset;
}

void Pine::Assets::Setup( )
{
	CreateAssetFactories( );
}

void Pine::Assets::Dispose( )
{
	Log::Message( "Disposing assets..." );

	for ( const auto& element : m_Assets ) 
	{
		auto asset = element.second;

		asset->Dispose( );

		delete asset;
	}

	m_Assets.clear( );
}

void Pine::Assets::SaveAssets( )
{
	for ( auto& element : m_Assets ) 
	{
		auto asset = element.second;

		if ( asset->GetUpdated( ) && !asset->IsMapped( ) && !asset->GetReadOnly( ) )
			asset->SaveToFile( );
	}
}

const std::unordered_map<std::string, Pine::IAsset*>& Pine::Assets::GetAssets( )
{
	return m_Assets;
}

bool Pine::Assets::DisposeAsset( const std::string& assetPath )
{
	if ( m_Assets.count( assetPath ) == 0 )
		return false;

	auto asset = m_Assets[ assetPath ];

	asset->Dispose( );

	delete asset;

	m_Assets.erase( assetPath );

	return true;
}

bool Pine::Assets::DisposeAsset( Pine::IAsset* asset )
{
	if ( !asset )
		return false;

	const std::string path = asset->GetPath( ).string( );

	asset->Dispose( );

	delete asset;

	m_Assets.erase( path );

	return true;
}
