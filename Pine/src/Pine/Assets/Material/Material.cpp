#include "Material.hpp"
#include "../../Core/Serialization/Serialization.hpp"

#include <fstream>

Pine::Material::Material( ) {
	m_Type = EAssetType::Material;
	m_Shader = Assets->GetAsset<Pine::Shader>( "Assets\\Engine\\Shaders\\Default.shr" );
}

glm::vec3& Pine::Material::DiffuseColor( ) {
	return m_DiffuseColor;
}

glm::vec3& Pine::Material::SpecularColor( ) {
	return m_SpecularColor;
}

glm::vec3& Pine::Material::AmbientColor( ) {
	return m_AmbientColor;
}

Pine::Texture2D* Pine::Material::GetDiffuse( ) const {
	return m_Diffuse;
}

Pine::Texture2D* Pine::Material::GetSpecular( ) const {
	return m_Specular;
}

Pine::Texture2D* Pine::Material::GetNormal( ) const
{
	return m_Normal;
}

void Pine::Material::SetDiffuse( Texture2D* texture ) {
	m_Diffuse = texture;
	m_Updated = true;
}

void Pine::Material::SetSpecular( Texture2D* texture ) {
	m_Specular = texture;
	m_Updated = true;
}

void Pine::Material::SetNormal( Texture2D* texture )
{
	m_Normal = texture;
}

float Pine::Material::GetTextureScale( ) const
{
	return m_TextureScale;
}

void Pine::Material::SetTextureScale( float scale )
{
	m_TextureScale = scale;
}

float Pine::Material::GetShininiess( ) const {
	return m_Shininiess;
}

void Pine::Material::SetShininiess( float shininiess ) {
	m_Shininiess = shininiess;
	m_Updated = true;
}

Pine::Shader* Pine::Material::GetShader( ) {
	return m_Shader;
}

void Pine::Material::SetShader( Shader* shader ) {
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

bool Pine::Material::LoadFromFile( ) {
	const auto j = Pine::Serialization::LoadJSONFromFile( m_FilePath.string( ) );

	try {

		m_Shader = dynamic_cast< Shader* >( Serialization::LoadAsset( j, "shader" ) );

		m_DiffuseColor = Serialization::LoadVec3( j, "DiffuseColor" );
		m_SpecularColor = Serialization::LoadVec3( j, "SpecularColor" );
		m_AmbientColor = Serialization::LoadVec3( j, "AmbientColor" );

		if ( j.contains( "shininess" ) )
			m_Shininiess = j[ "shininess" ].get<float>( );

		if ( j.contains( "textureScale" ) )
			m_TextureScale = j[ "textureScale" ].get<float>( );

		m_Diffuse = dynamic_cast< Texture2D* >( Serialization::LoadAsset( j, "diffuse" ) );
		m_Specular = dynamic_cast< Texture2D* >( Serialization::LoadAsset( j, "specularMap" ) );
		m_Normal = dynamic_cast< Texture2D* >( Serialization::LoadAsset( j, "normalMap" ) );

		// Fall back to default shader if shit goes wrong
		if ( m_Shader == nullptr )
		{
			m_Shader = Pine::Assets->GetAsset<Pine::Shader>( "Assets\\Engine\\Shaders\\Default.shr" );
		}

	}
	catch ( ... ) {

	}

	return true;
}

bool Pine::Material::SaveToFile( ) {
	if ( IsGenerated( ) )
		return false;

	nlohmann::json j;

	Serialization::SaveVec3( j[ "DiffuseColor" ], m_DiffuseColor );
	Serialization::SaveVec3( j[ "SpecularColor" ], m_SpecularColor );
	Serialization::SaveVec3( j[ "AmbientColor" ], m_AmbientColor );

	j[ "shininess" ] = m_Shininiess;
	j[ "textureScale" ] = m_TextureScale;

	Serialization::SaveAsset( j[ "diffuse" ], m_Diffuse );
	Serialization::SaveAsset( j[ "specularMap" ], m_Specular );
	Serialization::SaveAsset( j[ "normal" ], m_Specular );
	
	Serialization::SaveAsset( j[ "shader" ], m_Shader );

	std::ofstream stream( m_FilePath );

	stream << j;

	stream.close( );

	return true;
}

void Pine::Material::Dispose( ) {

}
