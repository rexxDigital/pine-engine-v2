#pragma once
#include <string>
#include <filesystem>

namespace Pine {
	class IAsset;
}

// Used to cache directories and files for rendering later
namespace Editor::DirectoryCache
{
	// Used for assets preview
	struct ItemCache_t {
		// We use std::filesystem::path here so we can extract more information without any hassle.
		std::filesystem::path path;

		// cached file stuff
		std::string name = "";

		// If this item is a directory or not, duh.
		bool isDirectory = false;

		// Used for stuff such as the engine asset folder.
		bool readOnly = false;

		// if this is an file and an asset has been loaded.
		Pine::IAsset* assetPointer = nullptr;
		
		// for directories, store all the children files & directories.
		std::vector<std::unique_ptr<ItemCache_t>> files;
		std::vector<std::unique_ptr<ItemCache_t>> directories;

		// store parent directory, it's nullptr for the root directory.
		ItemCache_t* parent = nullptr;
	};

	void SetRootDirectory(const std::string& rootDir);
	const std::string& GetRootDirectoryPath();

	ItemCache_t* GetRootDirectoryCache();
	
	void Refresh();
	
}
