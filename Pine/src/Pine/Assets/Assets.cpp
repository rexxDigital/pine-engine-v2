#include "Assets.hpp"
#include "../Core/Core.hpp"
#include "../Core/Log/Log.hpp"

#include "Blueprint/Blueprint.hpp"
#include "Invalid/InvalidAsset.hpp"
#include "Level/Level.hpp"
#include "Material/Material.hpp"
#include "Model/Model.hpp"
#include "Script/Script.hpp"
#include "Texture2D/Texture2D.hpp"
#include "Texture3D/Texture3D.hpp"

namespace {
	std::unordered_map<std::string, Pine::IAsset*> m_Assets;

	// Gets a EAssetType from common file extension types, for instance .obj -> Model
	Pine::EAssetType TranslateAssetType( const std::string& fileName ) {
		if ( Pine::String::EndsWith( fileName, ".mat" ) )
			return Pine::EAssetType::Material;
		if ( Pine::String::EndsWith( fileName, ".shr" ) )
			return Pine::EAssetType::Shader;
		if ( Pine::String::EndsWith( fileName, ".obj" ) || Pine::String::EndsWith( fileName, ".fbx" ) )
			return Pine::EAssetType::Model;
		if ( Pine::String::EndsWith( fileName, ".png" ) || Pine::String::EndsWith( fileName, ".jpg" ) )
			return Pine::EAssetType::Texture2D;
		if ( Pine::String::EndsWith( fileName, ".cmap" ) )
			return Pine::EAssetType::Texture3D;
		if ( Pine::String::EndsWith( fileName, ".bpt" ) )
			return Pine::EAssetType::Blueprint;
		if ( Pine::String::EndsWith( fileName, ".lvl" ) )
			return Pine::EAssetType::Level;
		if ( Pine::String::EndsWith( fileName, ".as" ) )
			return Pine::EAssetType::Script;

		return Pine::EAssetType::Invalid;
	}

	Pine::IAsset* CreateAssetType( Pine::EAssetType type ) {
		switch ( type ) {
		case Pine::EAssetType::Invalid:
			return new Pine::InvalidAsset( );
		case Pine::EAssetType::Material:
			return new Pine::Material( );
		case Pine::EAssetType::Shader:
			return new Pine::Shader( );
		case Pine::EAssetType::Model:
			return new Pine::Model( );
		case Pine::EAssetType::Texture2D:
			return new Pine::Texture2D( );
		case Pine::EAssetType::Texture3D:
			return new Pine::Texture3D( );
		case Pine::EAssetType::Blueprint:
			return new Pine::Blueprint( );
		case Pine::EAssetType::Level:
			return new Pine::Level( );
		case Pine::EAssetType::Script:
			return new Pine::Script( );
		default:
			return new Pine::InvalidAsset( );
		}
	}
}

Pine::IAsset* Pine::Assets::LoadFromFile( const std::string& filePath, bool readOnly ) {
	// ".asset" files are files that some assets use to store additional data for the asset,
	// since some asset files have standards, such as model files.
	if ( Pine::String::EndsWith( filePath, ".asset" ) )
		return nullptr;

	if ( m_Assets.count( filePath ) > 0 ) {
		auto asset = m_Assets[ filePath ];

		// Reload the file if it has been updated.
		if ( asset->HasBeenUpdated( ) ) {
			Log::Message( "Reloading file " + asset->GetFileName( ) + " since it has been updated." );

			// We will have to dispose the old stuff.
			// I am not too sure right now this reload system will work, since some variables will be the same.
			// The solution might be just to remove the old object completely and treat it as a new object.
			asset->Dispose( );

			// Load the asset again
			if ( !asset->LoadFromFile( ) ) {
				Log::Warning( "Failed to reload file, " + asset->GetFileName( ) );
				return nullptr;
			}

			// Mark it as reloaded
			asset->UpdateLastWriteTime( );
		}

		return m_Assets[ filePath ];
	}

	const auto type = TranslateAssetType( filePath );

	// Quick hack to prevent loading sky box images twice.
	if ( type == EAssetType::Texture2D && std::filesystem::exists( std::filesystem::path( filePath ).parent_path( ).string( ) + "\\IGNORE_TEXTURES" ) ) {
		return nullptr;
	}

	const auto asset = CreateAssetType( type );

	if ( !asset ) {
		return nullptr;
	}

	asset->SetFilePath( filePath );

	if ( !asset->LoadFromFile( ) ) {
		return nullptr;
	}

	asset->SetReadOnly( readOnly );
	asset->UpdateLastWriteTime( );

	m_Assets[ filePath ] = asset;

	return asset;
}

int Pine::Assets::LoadFromDirectory( const std::string& directoryPath, bool readOnly ) {
	uint32_t loadedAssets = 0;

	for ( const auto& dirEntry : std::filesystem::recursive_directory_iterator( directoryPath ) ) {
		if ( dirEntry.is_directory( ) )
			continue;

		if ( LoadFromFile( dirEntry.path( ).string( ), readOnly ) ) {
			loadedAssets++;
		}
	}

	if ( loadedAssets > 0 )
		Log::Message( "Successfully loaded " + std::to_string( loadedAssets ) + " asset(s)." );
	else
		Log::Warning( "Loaded 0 assets from the directory " + directoryPath );

	return loadedAssets;
}

Pine::IAsset* Pine::Assets::GetAsset( const std::string& assetPath ) {
	if ( m_Assets.count( assetPath ) == 0 )
		return nullptr;

	return m_Assets[ assetPath ];
}

void Pine::Assets::Dispose( ) {
	Log::Message( "Disposing assets..." );

	for ( auto element : m_Assets ) {
		auto asset = element.second;

		asset->Dispose( );

		delete asset;
	}
}

void Pine::Assets::SaveAssets( )
{
	for ( auto& element : m_Assets ) {
		auto asset = element.second;

		if ( asset->GetUpdated( ) )
			asset->SaveToFile( );
	}
}

const std::unordered_map<std::string, Pine::IAsset*>& Pine::Assets::GetAssets( ) {
	return m_Assets;
}

bool Pine::Assets::DisposeAsset( const std::string& assetPath ) {
	if ( m_Assets.count( assetPath ) == 0 )
		return false;

	auto asset = m_Assets[ assetPath ];

	asset->Dispose( );

	delete asset;

	m_Assets.erase( assetPath );

	return true;
}

bool Pine::Assets::DisposeAsset( Pine::IAsset* asset ) {
	if ( !asset )
		return false;

	const std::string path = asset->GetPath( ).string( );

	asset->Dispose( );

	delete asset;

	m_Assets.erase( path );

	return true;
}
