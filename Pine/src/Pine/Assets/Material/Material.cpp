#include "Material.hpp"
#include "../../Core/Serialization/Serialization.hpp"

#include <fstream>

Pine::Material::Material( )
{
	m_Type = AssetType::Material;
	m_Shader = Assets->GetAsset<Shader>( "Assets/Engine/Shaders/Default.shr" );
}

glm::vec3& Pine::Material::DiffuseColor( )
{
	return m_DiffuseColor;
}

glm::vec3& Pine::Material::SpecularColor( )
{
	return m_SpecularColor;
}

glm::vec3& Pine::Material::AmbientColor( )
{
	return m_AmbientColor;
}

Pine::Texture2D* Pine::Material::GetDiffuse( ) const
{
	return m_Diffuse.Get( );
}

Pine::Texture2D* Pine::Material::GetSpecular( ) const
{
	return m_Specular.Get( );
}

Pine::Texture2D* Pine::Material::GetNormal( ) const
{
	return m_Normal.Get( );
}

void Pine::Material::SetDiffuse( Texture2D* texture )
{
	m_Diffuse = texture;
	m_Updated = true;
}

void Pine::Material::SetSpecular( Texture2D* texture )
{
	m_Specular = texture;
	m_Updated = true;
}

void Pine::Material::SetNormal( Texture2D* texture )
{
	m_Normal = texture;
	m_Updated = true;
}

float Pine::Material::GetTextureScale( ) const
{
	return m_TextureScale;
}

void Pine::Material::SetTextureScale( float scale )
{
	m_TextureScale = scale;
	m_Updated = true;
}

float Pine::Material::GetShininess( ) const
{
	return m_Shininess;
}

void Pine::Material::SetShininess( float Shininess )
{
	m_Shininess = Shininess;
	m_Updated = true;
}

Pine::Shader* Pine::Material::GetShader( )
{
	return m_Shader.Get( );
}

void Pine::Material::SetShader( Shader* shader )
{
	m_Shader = shader;
}

bool Pine::Material::IsGenerated( ) const
{
	return m_IsGenerated;
}

void Pine::Material::SetGenerated( bool generated )
{
	m_IsGenerated = generated;
}

Pine::MatRenderingMode Pine::Material::GetRenderingMode( ) const
{
	return m_RenderingMode;
}

void Pine::Material::SetRenderingMode( MatRenderingMode mode )
{
	m_RenderingMode = mode;
	m_Updated = true;
}

std::uint32_t Pine::Material::GetRenderFlags( ) const
{
	return m_RenderFlags;
}

void Pine::Material::SetRenderFlags( std::uint32_t flags )
{
	m_RenderFlags = flags;
	m_Updated = true;
}

std::uint32_t Pine::Material::GetShaderProperties( ) const
{
	return m_ShaderProperties;
}

void Pine::Material::SetShaderProperties( std::uint32_t flags )
{
	m_ShaderProperties = flags;
	m_Updated = true;
}

bool Pine::Material::LoadFromFile( )
{
	const auto j = Pine::Serialization::LoadJSONFromFile( m_FilePath.string( ) );

	try
	{

		m_Shader = dynamic_cast< Shader* >( Serialization::LoadAsset( j, "shader" ) );

		m_DiffuseColor = Serialization::LoadVec3( j, "DiffuseColor" );
		m_SpecularColor = Serialization::LoadVec3( j, "SpecularColor" );
		m_AmbientColor = Serialization::LoadVec3( j, "AmbientColor" );

		if ( j.contains( "shininess" ) )
			m_Shininess = j[ "shininess" ].get<float>( );

		if ( j.contains( "textureScale" ) )
			m_TextureScale = j[ "textureScale" ].get<float>( );

		if ( j.contains( "renderMode" ) )
			m_RenderingMode = j[ "renderMode" ].get<MatRenderingMode>( );

		if ( j.contains( "shaderProperties" ) )
			m_ShaderProperties = std::stoul( j[ "shaderProperties" ].get<std::string>(  ) );

		if ( j.contains( "renderFlags" ) )
			m_RenderFlags = std::stoul( j[ "renderFlags" ].get<std::string>( ) );

		m_Diffuse = dynamic_cast< Texture2D* >( Serialization::LoadAsset( j, "diffuse" ) );
		m_Specular = dynamic_cast< Texture2D* >( Serialization::LoadAsset( j, "specularMap" ) );
		m_Normal = dynamic_cast< Texture2D* >( Serialization::LoadAsset( j, "normalMap" ) );

		// Fall back to default shader if shit goes wrong
		if ( m_Shader == nullptr )
		{
			m_Shader = Pine::Assets->GetAsset<Pine::Shader>( "Assets/Engine/Shaders/Default.shr" );
		}

	}
	catch ( ... )
	{

	}

	return true;
}

bool Pine::Material::SaveToFile( )
{
	if ( IsGenerated( ) )
		return false;

	nlohmann::json j;

	Serialization::SaveVec3( j[ "DiffuseColor" ], m_DiffuseColor );
	Serialization::SaveVec3( j[ "SpecularColor" ], m_SpecularColor );
	Serialization::SaveVec3( j[ "AmbientColor" ], m_AmbientColor );

	j[ "shininess" ] = m_Shininess;
	j[ "textureScale" ] = m_TextureScale;
	j[ "renderMode" ] = m_RenderingMode;
	j[ "renderFlags" ] = std::to_string( m_RenderFlags );
	j[ "shaderProperties" ] = std::to_string( m_ShaderProperties );

	Serialization::SaveAsset( j[ "diffuse" ], m_Diffuse.Get( ) );
	Serialization::SaveAsset( j[ "specularMap" ], m_Specular.Get( ) );
	Serialization::SaveAsset( j[ "normal" ], m_Specular.Get( ) );

	Serialization::SaveAsset( j[ "shader" ], m_Shader.Get( ) );

	std::ofstream stream( m_FilePath );

	stream << j;

	stream.close( );

	return true;
}

void Pine::Material::Dispose( )
{

}
