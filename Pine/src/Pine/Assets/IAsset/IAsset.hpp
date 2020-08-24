#pragma once
#include <filesystem>

namespace Pine
{
	enum class EAssetType
	{
		Invalid,
		Material,
		Mesh,
		Model,
		Shader,
		Texture
	};

	class IAsset
	{
	protected:
		std::filesystem::path m_FilePath;
		EAssetType m_Type = EAssetType::Invalid;
	public:
		virtual ~IAsset() = default;

		void SetFilePath(const std::string& str);
		const std::filesystem::path& GetPath() const;

		EAssetType GetType() const;

		virtual bool LoadFromFile() = 0;
		virtual bool SaveToFile() = 0;
		virtual void Dispose() = 0;
	};
}