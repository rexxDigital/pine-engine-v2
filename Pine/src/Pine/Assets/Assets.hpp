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

	bool DisposeAsset( const std::string& assetPath );
	bool DisposeAsset( Pine::IAsset* asset );

	template <class T>
	T* GetAsset( const std::string& assetPath ) {
		auto asset = GetAsset( assetPath );
		return dynamic_cast< T* >( asset );
	}

	// Disposes all loaded assets
	void Dispose( );

	const std::unordered_map<std::string, IAsset*>& GetAssets( );
}