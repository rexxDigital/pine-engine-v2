#pragma once
#include <filesystem>

namespace Pine { class FrameBuffer; }

namespace Pine {
	enum class EAssetType {
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

	inline const char* SAssetType[ ] = {
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

	class IAsset {
	protected:
		std::filesystem::path m_FilePath;

		std::string m_FileName = "";
		std::string m_AbsoulteFilePath = "";
		
		EAssetType m_Type = EAssetType::Invalid;
		
		std::chrono::system_clock::duration m_LastWriteTime;
	public:
		virtual ~IAsset( ) = default;

		void SetFilePath( const std::string& str );
		const std::filesystem::path& GetPath( ) const;

		const std::string& GetFileName( ) const;
		const std::string& GetAbsoluteFilePath( ) const;

		EAssetType GetType( ) const;

		void UpdateLastWriteTime( );
		bool HasBeenUpdated( ) const;

		virtual bool LoadFromFile( ) = 0;
		virtual bool SaveToFile( ) = 0;
		virtual void Dispose( ) = 0;
	};
}