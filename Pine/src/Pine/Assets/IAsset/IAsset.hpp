#pragma once
#include <filesystem>

#define PE_

namespace Pine { class FrameBuffer; }

namespace Pine
{

	enum class AssetType
	{
		Invalid,
		Material,
		Mesh,
		Model,
		Shader,
		Texture2D,
		Texture3D,
		Blueprint,
		Level,
		Script,
		Terrain
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
		"Script",
		"Terrain"
	};

	enum class AssetState
	{
		Unloaded,
		Prepared,
		Loaded
	};

	class IAsset
	{
	protected:
		std::filesystem::path m_FilePath;

		std::string m_FileName = "";
		std::string m_AbsoluteFilePath = "";

		AssetType m_Type = AssetType::Invalid;

		std::chrono::system_clock::duration m_LastWriteTime;

		bool m_ReadOnly = false;
		bool m_Updated = false;
		bool m_IsMapped = false;

        int m_ReferenceCount = 0;
        bool m_Deleted = false;

		bool m_InMemory = false;
		void* m_DataPtr = nullptr;

		AssetState m_AssetState;
	public:
		virtual ~IAsset( ) = default;

		void SetFilePath( const std::string& str );
		const std::filesystem::path& GetPath( ) const;

		const std::string& GetFileName( ) const;
		const std::string& GetAbsoluteFilePath( ) const;

		AssetType GetType( ) const;

		void UpdateLastWriteTime( );
		bool HasBeenUpdated( ) const;

		bool GetReadOnly( ) const; 
		void SetReadOnly( bool value ); 

		bool GetUpdated( ) const;
		void SetUpdated( bool value );

		bool IsMapped( ) const;
		void SetMapped( bool value );

        bool IsDeleted( ) const;
        void SetDeleted( bool value );

        void IncreaseReference( );
        void DecreaseReference( );

		AssetState GetAssetState( ) const;

		virtual bool LoadFromFile( ) = 0;
		virtual bool SaveToFile( ) = 0;
		virtual void Dispose( ) = 0;
	};

    // This would have never been required if I were using smart pointers in the first place, but ah well, I don't mind so far.
    template <class T>
    struct AssetContainer
    {
        mutable T m_Asset = nullptr;

        T Get( ) const
        {
            if ( m_Asset )
            {
                if ( reinterpret_cast<IAsset*>( m_Asset )->IsDeleted( ) )
                {
                    m_Asset = nullptr;
                }
            }

            return m_Asset;
        }

        AssetContainer& operator=(IAsset* asset)
        {
            m_Asset = static_cast<T>(asset);

            return *this;
        }

        inline bool operator==(const IAsset* b)
        {
            return m_Asset == b;
        }
    };

}
