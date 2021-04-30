#pragma once

#include "IAsset/IAsset.hpp"
#include <memory>
#include <unordered_map>

namespace Pine::Assets {

	// Loads a asset file from the disk
	// Uses the file extension to determine asset type
	// Returns nullptr on failure.
	IAsset* LoadFromFile( const std::string& filePath, bool readOnly = false );

	// Loads all asset in a directory recursively, returns amount of assets loaded.
	int LoadFromDirectory( const std::string& directoryPath, bool readOnly = false );

	// Returns a loaded asset by file path, your job to cast to the right type.
	IAsset* GetAsset( const std::string& assetPath );

	// Fakes an asset being loaded at a specific path, will also make the asset manager handle disposing of assets.
	void MapAsset( IAsset* asset, const std::string& fakePath );
	
	bool DisposeAsset( const std::string& assetPath );
	bool DisposeAsset( Pine::IAsset* asset );

	template <class T>
	T* GetAsset( const std::string& assetPath ) {
		auto asset = GetAsset( assetPath );
		return dynamic_cast< T* >( asset );
	}

	void Setup( );
	
	// Disposes all loaded assets
	void Dispose( );

	// Calls SaveToFile on all assets
	void SaveAssets( );

	const std::unordered_map<std::string, IAsset*>& GetAssets( );
}