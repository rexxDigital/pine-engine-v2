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
		Texture3D
	};

	inline const char* SAssetType[] = {
		"Invalid",
		"Material",
		"Mesh",
		"Model",
		"Shader",
		"Texture2D",
		"Texture3D (Cube map)"
	};

	class IAsset
	{
	protected:
		std::filesystem::path m_FilePath;
		EAssetType m_Type = EAssetType::Invalid;
		FrameBuffer* m_PreviewFrameBuffer = nullptr;
	public:
		virtual ~IAsset() = default;

		void SetFilePath(const std::string& str);
		const std::filesystem::path& GetPath() const;

		EAssetType GetType() const;
		
		// For asset previews in the editor, GetAssetPreview should return texture id.
		bool HasAvailablePreview() const;
		virtual const int GetAssetPreview() const;

		virtual bool LoadFromFile() = 0;
		virtual bool SaveToFile() = 0;
		virtual void Dispose() = 0;
		virtual void GenerateAssetPreview();
	};
}