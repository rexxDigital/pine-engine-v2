#include "Assets.hpp"
#include "../Core/Core.hpp"
#include "../Core/Log/Log.hpp"
#include "Blueprint/Blueprint.hpp"

#include "Material/Material.hpp"
#include "Model/Model.hpp"
#include "Texture2D/Texture2D.hpp"

#include "Texture3D/Texture3D.hpp"


namespace {
	std::unordered_map<std::string, Pine::IAsset*> m_Assets;

	// Gets a EAssetType from common file extension types, for instance .obj -> Model
	Pine::EAssetType TranslateAssetType(const std::string& fileName) {
		if (Pine::String::EndsWith(fileName, ".mat"))
			return Pine::EAssetType::Material;
		if (Pine::String::EndsWith(fileName, ".shr"))
			return Pine::EAssetType::Shader;
		if (Pine::String::EndsWith(fileName, ".obj") || Pine::String::EndsWith( fileName, ".fbx" ) )
			return Pine::EAssetType::Model;
		if (Pine::String::EndsWith(fileName, ".png") || Pine::String::EndsWith(fileName, ".jpg"))
			return Pine::EAssetType::Texture2D;
		if (Pine::String::EndsWith(fileName, ".cmap"))
			return Pine::EAssetType::Texture3D;
		if ( Pine::String::EndsWith( fileName, ".bpt" ) )
			return Pine::EAssetType::Blueprint;
		
		return Pine::EAssetType::Invalid;
	}

	Pine::IAsset* CreateAssetType(Pine::EAssetType type) {
		switch (type) {
		case Pine::EAssetType::Invalid:
			return nullptr;
		case Pine::EAssetType::Material:
			return new Pine::Material();
		case Pine::EAssetType::Shader:
			return new Pine::Shader();
		case Pine::EAssetType::Model:
			return new Pine::Model();
		case Pine::EAssetType::Texture2D:
			return new Pine::Texture2D();
		case Pine::EAssetType::Texture3D:
			return new Pine::Texture3D();
		case Pine::EAssetType::Blueprint:
			return new Pine::Blueprint( );
		default:
			return nullptr;
		}
	}
}

Pine::IAsset* Pine::Assets::LoadFromFile(const std::string& filePath) {
	if (m_Assets.count(filePath) > 0) {
		return m_Assets[filePath];
	}

	const auto type = TranslateAssetType(filePath);

	if (type == EAssetType::Invalid) {
		return nullptr;
	}

	// Quick hack to prevent loading sky box images twice.
	if (type == EAssetType::Texture2D && std::filesystem::exists(std::filesystem::path(filePath).parent_path().string() + "\\IGNORE_TEXTURES")) {
		return nullptr;
	}

	const auto asset = CreateAssetType(type);

	if (!asset) {
		return nullptr;
	}

	asset->SetFilePath(filePath);

	if (!asset->LoadFromFile()) {
		// We have a strict "please report the issue in the LoadFromFile method", no logging.
		return nullptr;
	}

	m_Assets[filePath] = asset;

	return asset;
}

int Pine::Assets::LoadFromDirectory(const std::string& directoryPath) {
	uint32_t loadedAssets = 0;

	for (const auto& dirEntry : std::filesystem::recursive_directory_iterator(directoryPath)) {
		if (dirEntry.is_directory())
			continue;

		if (LoadFromFile(dirEntry.path().string())) {
			loadedAssets++;
		}
	}

	if (loadedAssets > 0)
		Log::Message("Successfully loaded " + std::to_string(loadedAssets) + " asset(s).");
	else
		Log::Warning("Loaded 0 assets from the directory " + directoryPath);

	return loadedAssets;
}

Pine::IAsset* Pine::Assets::GetAsset(const std::string& assetPath) {
	if (m_Assets.count(assetPath) == 0)
		return nullptr;

	return m_Assets[assetPath];
}

void Pine::Assets::Dispose() {
	Log::Message("Disposing assets...");

	for (auto element : m_Assets) {
		auto asset = element.second;

		asset->Dispose();
	}
}

const std::unordered_map<std::string, Pine::IAsset*>& Pine::Assets::GetAssets()
{
	return m_Assets;
}
