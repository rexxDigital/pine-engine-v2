#include "Assets.hpp"

#include <utility>
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

namespace Pine
{

	class CAssets : public IAssets
	{
	private:
		std::unordered_map<std::string, Pine::IAsset*> m_Assets;

		// This is written to translate file extensions to loaded asset objects in memory.

		struct AssetFactory_t
		{
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

			m_AssetFactories.push_back( AssetFactory_t( { { ".mat" }, Pine::EAssetType::Material, [ ] ( ) { return new Pine::Material( ); } } ) );
			m_AssetFactories.push_back( AssetFactory_t( { { ".shr" }, Pine::EAssetType::Shader, [ ] ( ) { return new Pine::Shader( ); } } ) );
			m_AssetFactories.push_back( AssetFactory_t( { { ".obj", ".fbx", ".3d", ".3ds", ".stl", ".dae", ".stp", ".wrl", ".ply" }, Pine::EAssetType::Model, [ ] ( ) { return new Pine::Model( ); } } ) );
			m_AssetFactories.push_back( AssetFactory_t( { { ".png", ".jpg", ".jpeg", ".tga", ".bmp", ".psd", ".gif" }, Pine::EAssetType::Texture2D, [ ] ( ) { return new Pine::Texture2D( ); } } ) );
			m_AssetFactories.push_back( AssetFactory_t( { { ".cmap" }, Pine::EAssetType::Texture3D, [ ] ( ) { return new Pine::Texture3D( ); } } ) );
			m_AssetFactories.push_back( AssetFactory_t( { { ".bpt" }, Pine::EAssetType::Blueprint, [ ] ( ) { return new Pine::Blueprint( ); } } ) );
			m_AssetFactories.push_back( AssetFactory_t( { { ".lvl" }, Pine::EAssetType::Level, [ ] ( ) { return new Pine::Level( ); } } ) );
			m_AssetFactories.push_back( AssetFactory_t( { { ".as" }, Pine::EAssetType::Script, [ ] ( ) { return new Pine::Script( ); } } ) );
			m_AssetFactories.push_back( AssetFactory_t( { { ".ter" }, Pine::EAssetType::Terrain, [ ] ( ) { return new Pine::Terrain( ); } } ) );

			Pine::Log->Message( "Loaded " + std::to_string( m_AssetFactories.size( ) ) + " asset factories." );
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

	public:

		Pine::IAsset* LoadFromFile( const std::string& filePath, bool readOnly ) override
		{
			// ".asset" files are files that some assets use to store additional data for the asset,
			// since some asset files have standards, such as model files.
			if ( Pine::String::EndsWith( filePath, ".asset" ) )
				return nullptr;

			if ( m_Assets.count( filePath ) > 0 )
			{
				const auto asset = m_Assets[ filePath ];

				if ( asset == nullptr )
					return nullptr;

				// Reload the file if it has been updated, do it a bit differently for shaders though.
				if ( asset->GetType( ) == EAssetType::Shader )
				{
					if ( const auto shader = dynamic_cast< Pine::Shader* >( asset ) )
					{
						bool shaderFilesUpdated = false;

						for ( const auto shaderFileAsset : shader->GetAttachedShaderFiles( ) )
						{
							if ( shaderFileAsset->HasBeenUpdated( ) )
							{
								shaderFilesUpdated = true;
								shaderFileAsset->UpdateLastWriteTime( );
							}
						}

						if ( shaderFilesUpdated )
						{
							Log->Message( "Reloading shader " + asset->GetFileName( ) + " since it's files has been updated." );

							shader->Dispose( );
							shader->LoadFromFile( );

							return asset;
						}
					}
				}

				// Reload the file if it has been updated.
				if ( !asset->IsMapped( ) && asset->HasBeenUpdated( ) )
				{
					Log->Message( "Reloading file " + asset->GetFileName( ) + " since it has been updated." );

					// Calling dispose *should* erase all the old garbage, so we can call LoadFromFile without any issues.
					// If this no longer works in the future with more "advanced" and cool asset types, we should just completely remove
					// the old object and create a new one. (memset & initialize again though so the ptr is the same please!)
					asset->Dispose( );

					// Load the asset again
					if ( !asset->LoadFromFile( ) )
					{
						Log->Warning( "Failed to reload file, " + asset->GetFileName( ) );
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

			Log->Debug( "Loading asset '" + filePath + "'..." );

			if ( !asset->LoadFromFile( ) )
			{
				return nullptr;
			}

			asset->SetReadOnly( readOnly );
			asset->UpdateLastWriteTime( );

			m_Assets[ filePath ] = asset;

			return asset;
		}

		int LoadFromDirectory( const std::string& directoryPath, bool readOnly ) override
		{
			int loadedAssets = 0;

			if ( !std::filesystem::exists( directoryPath ) )
			{
				Log->Error( "Failed to load assets from, " + directoryPath + ", directory does not exist." );
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
				Log->Warning( "Loaded 0 assets from the directory " + directoryPath );

			return loadedAssets;
		}

		Pine::IAsset* GetAsset( const std::string& assetPath ) override
		{
			if ( m_Assets.count( assetPath ) == 0 )
				return nullptr;

			return m_Assets[ assetPath ];
		}

		void MapAsset( IAsset* asset, const std::string& fakePath ) override
		{
			if ( Pine::String::EndsWith( fakePath, ".asset" ) )
			{
				Log->Error( "Failed to map '" + fakePath + "', cannot map a '.asset' file." );

				return;
			}

			asset->SetFilePath( fakePath );
			asset->SetReadOnly( true );
			asset->SetMapped( true );

			m_Assets[ fakePath ] = asset;
		}

		void Setup( ) override
		{
			CreateAssetFactories( );
		}

		void Dispose( ) override
		{
			Log->Message( "Disposing assets..." );

			for ( const auto& element : m_Assets )
			{
				const auto asset = element.second;

				asset->Dispose( );

				delete asset;
			}

			m_Assets.clear( );
		}

		void SaveAssets( ) override
		{
			for ( const auto& element : m_Assets )
			{
				const auto asset = element.second;

				if ( asset->GetUpdated( ) && !asset->IsMapped( ) && !asset->GetReadOnly( ) )
					asset->SaveToFile( );
			}
		}

		bool DisposeAsset( const std::string& assetPath ) override
		{
			if ( m_Assets.count( assetPath ) == 0 )
				return false;

			const auto asset = m_Assets[ assetPath ];

			asset->Dispose( );

			delete asset;

			m_Assets.erase( assetPath );

			return true;
		}

		bool DisposeAsset( Pine::IAsset* asset ) override
		{
			if ( !asset )
				return false;

			const std::string path = asset->GetPath( ).string( );

			asset->Dispose( );

			delete asset;

			m_Assets.erase( path );

			return true;
		}

		const std::unordered_map<std::string, IAsset*>& GetAssets( ) override
		{
			return m_Assets;
		}

	};

	IAssets* CreateAssetsInterface( )
	{
		return new CAssets( );
	}
}
