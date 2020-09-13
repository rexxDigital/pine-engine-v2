#pragma once
#include <filesystem>

namespace Pine { class FrameBuffer; }

namespace Pine
{
	enum class EAssetType
	{
		Invalid,
		Material,
		Mesh,
		Model,
		Shader,
		Texture2D,
		Texture3D,
		Blueprint,
		Level
	};

	inline const char* SAssetType[] = {
		"Invalid",
		"Material",
		"Mesh",
		"Model",
		"Shader",
		"Texture2D",
		"Texture3D (Cube map)",
		"Blueprint",
		"Level"
	};

	class IAsset
	{
	protected:
		std::filesystem::path m_FilePath;
		std::string m_FileName = "";
		EAssetType m_Type = EAssetType::Invalid;
		FrameBuffer* m_PreviewFrameBuffer = nullptr;
	public:
		virtual ~IAsset() = default;

		void SetFilePath(const std::string& str);
		const std::filesystem::path& GetPath() const;

		const std::string& GetFileName() const;
		
		EAssetType GetType() const;

		virtual bool LoadFromFile() = 0;
		virtual bool SaveToFile() = 0;
		virtual void Dispose() = 0;
	};
}