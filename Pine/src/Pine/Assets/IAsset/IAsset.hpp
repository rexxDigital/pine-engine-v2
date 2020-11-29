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
		Level,
		Script
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
		"Level",
		"Script"
	};

	class IAsset {
	protected:
		std::filesystem::path m_FilePath;

		std::string m_FileName = "";
		std::string m_AbsoulteFilePath = "";
		
		EAssetType m_Type = EAssetType::Invalid;
		
		std::chrono::system_clock::duration m_LastWriteTime;

		bool m_ReadOnly = false;
		bool m_Updated = false;
	public:
		virtual ~IAsset( ) = default;

		void SetFilePath( const std::string& str );
		const std::filesystem::path& GetPath( ) const;

		const std::string& GetFileName( ) const;
		const std::string& GetAbsoluteFilePath( ) const;

		EAssetType GetType( ) const;

		void UpdateLastWriteTime( );
		bool HasBeenUpdated( ) const;

		bool GetReadOnly( ) const;
		void SetReadOnly( bool value );

		bool GetUpdated( ) const;
		void SetUpdated( bool value );
		
		virtual bool LoadFromFile( ) = 0;
		virtual bool SaveToFile( ) = 0;
		virtual void Dispose( ) = 0;
	};
}