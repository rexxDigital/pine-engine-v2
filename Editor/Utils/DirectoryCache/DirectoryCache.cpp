#include "DirectoryCache.hpp"
#include <memory>

#include "Pine/Assets/Assets.hpp"

// Forgive me.
using namespace Editor::DirectoryCache;

namespace
{

	std::string g_RootDirectoryPath = "";

	std::unique_ptr<ItemCache_t> g_RootDirectory = nullptr;

	std::unique_ptr<ItemCache_t> CreateDirectoryCache(const std::string& dir, ItemCache_t* parent) {
		auto dirCache = std::make_unique<ItemCache_t>();

		dirCache->path = dir;
		dirCache->name = std::filesystem::path(dir).filename().string();
		dirCache->isDirectory = std::filesystem::is_directory(dir);
		dirCache->parent = parent;

		if (!dirCache->isDirectory) {
			dirCache->assetPointer = Pine::Assets::GetAsset(dir);
		}
		else {
			if (dirCache->parent != nullptr) {
				auto parentDir = std::make_unique<ItemCache_t>();

				parentDir->path = dirCache->parent->path;
				parentDir->name = "...";
				parentDir->isDirectory = true;
				parentDir->parent = dirCache->parent;

				dirCache->directories.push_back(std::move(parentDir));
			}

			for (auto item : std::filesystem::directory_iterator(dir)) {
				if (item.is_directory())
					dirCache->directories.push_back(CreateDirectoryCache(item.path().string(), dirCache.get()));
				else
					dirCache->files.push_back(CreateDirectoryCache(item.path().string(), dirCache.get()));
			}
		}

		return std::move(dirCache);
	}
	
}

void Editor::DirectoryCache::SetRootDirectory(const std::string& rootDir)
{
	g_RootDirectoryPath = rootDir;
}

const std::string& Editor::DirectoryCache::GetRootDirectoryPath()
{
	return g_RootDirectoryPath;
}

ItemCache_t* Editor::DirectoryCache::GetRootDirectoryCache()
{
	return g_RootDirectory.get();
}

void Editor::DirectoryCache::Refresh()
{
	g_RootDirectory = CreateDirectoryCache(g_RootDirectoryPath, nullptr);
}
