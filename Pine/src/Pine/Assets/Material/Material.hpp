#pragma once
#include "../IAsset/IAsset.hpp"
#include "../Shader/Shader.hpp"
#include "../Texture2D/Texture2D.hpp"

#include <glm/vec3.hpp>

namespace Pine
{

	enum class MatRenderingMode
	{
		Opaque, // For non transparent textures
		Discard, // For either non transparent or fully transparent textures
		Transparent, // For semi transparent textures,
		Size
	};

	enum RenderFlags
	{
		DisableBackfaceCulling = ( 1 << 0 ),
		RenderWireframe = ( 1 << 1 )
	};

	class Material : public IAsset
	{
	private:
		glm::vec3 m_DiffuseColor = glm::vec3( 1.f, 1.f, 1.f );
		glm::vec3 m_SpecularColor = glm::vec3( 0.f, 0.f, 0.f );
		glm::vec3 m_AmbientColor = glm::vec3( 0.5f, 0.5f, 0.5f );

        AssetContainer<Texture2D*> m_Diffuse;
        AssetContainer<Texture2D*> m_Specular;
        AssetContainer<Texture2D*> m_Normal;

		AssetContainer<Shader*> m_Shader;

		MatRenderingMode m_RenderingMode = MatRenderingMode::Opaque;

		std::uint32_t m_ShaderProperties = 0;
		std::uint32_t m_RenderFlags = 0;

		float m_Shininess = 16.f;
		float m_TextureScale = 1.f;

		bool m_IsGenerated = false;
	public:
		Material( );

		glm::vec3& DiffuseColor( );
		glm::vec3& SpecularColor( );
		glm::vec3& AmbientColor( );

		Texture2D* GetDiffuse( ) const;
		Texture2D* GetSpecular( ) const;
		Texture2D* GetNormal( ) const;

		void SetDiffuse( Texture2D* texture );
		void SetSpecular( Texture2D* texture );
		void SetNormal( Texture2D* texture );

		float GetTextureScale( ) const;
		void SetTextureScale( float scale );

		float GetShininess( ) const;
		void SetShininess( float Shininess );

		Shader* GetShader( );
		void SetShader( Shader* shader );

		bool IsGenerated( ) const;
		void SetGenerated( bool generated );

		MatRenderingMode GetRenderingMode( ) const;
		void SetRenderingMode( MatRenderingMode mode );

		std::uint32_t GetRenderFlags( ) const;
		void SetRenderFlags( std::uint32_t flags );

		std::uint32_t GetShaderProperties( ) const;
		void SetShaderProperties( std::uint32_t flags );

		bool LoadFromFile( ) override;
		bool SaveToFile( ) override;
		void Dispose( ) override;
	};

}