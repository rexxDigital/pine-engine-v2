#pragma once

#include "IAsset/IAsset.hpp"
#include "../Core/Interfaces/Interfaces.hpp"

#include <memory>
#include <unordered_map>

/*
 * BRAINSTORMING: In OpenGL we can only actually upload stuff to the GPU from the main OpenGL thread, it's more or less impossible to try to upload a texture from a separate thread,
 * now this obviously doesn't completely "kill" the purpose of multi threading the asset loading part, since a lot of times the time consuming part of asset loading is loading and parsing the data
 * from the disk, so we can do all of that in a separate thread and then when the data is actually ready, throw it all at the main thread and upload it to the GPU. For assets that doesn't include the
 * GPU this doesn't really matter of course.
 *
 * Another question we'll have to answer is how do we split the workload of loading assets into different threads, do we split the amount of files to be processed into equal large chunks for different
 * threads to process? Another possibility could be quickly estimating the amount of time an asset would take to load, by example maybe looking at the file size or something, but I think that would just
 * end up over complicating things for something that probably wouldn't help that much. While I am writing this, multi threaded asset loading isn't really required since the engine is still super small,
 * and I don't know if I'll ever come to the point where I actually have to do this.
 */

/*
 * Another important thing for a game engine to be able to handle is "acknowledging" an asset's existence, but not actually loading it into memory unless it's needed for the level. Right now, this engine
 * will just load everything it sees within a directory and leave it at that. I was thinking of rewriting parts of the engine to account for an "m_InMemory" boolean, and when an asset is actually requested
 * we'll load it, and to add onto load assets that's used within a Level after a level is loaded.
 */

namespace Pine
{

	class IAssets : public IInterface
	{
	public:
		// Loads a asset file from the disk
		// Uses the file extension to determine asset type
		// Returns nullptr on failure.
		virtual IAsset* LoadFromFile( const std::string& filePath, bool readOnly = false ) = 0;

		// Loads all asset in a directory recursively, returns amount of assets loaded.
		virtual int LoadFromDirectory( const std::string& directoryPath, bool readOnly = false ) = 0;

		// Returns a loaded asset by file path, your job to cast to the right type.
		virtual IAsset* GetAsset( const std::string& assetPath ) = 0;

		template <class T>
		T* GetAsset( const std::string& assetPath ) {
			auto asset = GetAsset( assetPath );
			return dynamic_cast< T* >( asset );
		}

		// Fakes an asset being loaded at a specific path, will also make the asset manager handle disposing of assets.
		virtual void MapAsset( IAsset* asset, const std::string& fakePath ) = 0;

		virtual bool DisposeAsset( const std::string& assetPath ) = 0;
		virtual bool DisposeAsset( Pine::IAsset* asset ) = 0;

		// This will go through all assets and save them to their original file if it has been modified in any way, this shouldn't really be used by a game runtime or something similar, it's used
		// by the editor so the user can customize an asset and save them. 
		virtual void SaveAssets( ) = 0;

		virtual const std::unordered_map<std::string, IAsset*>& GetAssets( ) = 0;
	};

}
