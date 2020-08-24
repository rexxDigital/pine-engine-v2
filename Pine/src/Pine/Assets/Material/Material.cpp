#include "Material.hpp"
#include "../../Core/Serialization/Serialization.hpp"

Pine::Material::Material()
{
	m_Type = EAssetType::Material;
	m_Shader = Assets::GetAsset<Pine::Shader>("Engine\\Shaders\\Default.shr");
}

glm::vec3& Pine::Material::DiffuseColor() {
	return m_DiffuseColor;
}

glm::vec3& Pine::Material::SpecularColor() {
	return m_SpecularColor;
}

glm::vec3& Pine::Material::AmbientColor() {
	return m_AmbientColor;
}

Pine::Texture* Pine::Material::GetDiffuse() const {
	return m_Diffuse;
}

Pine::Texture* Pine::Material::GetSpecular() const {
	return m_Specular;
}

void Pine::Material::SetDiffuse(Texture* texture) {
	m_Diffuse = texture;
}

void Pine::Material::SetSpecular(Texture* texture) {
	m_Specular = texture;
}

float Pine::Material::GetShininiess() const {
	return m_Shininiess;
}

void Pine::Material::SetShininiess(float shininiess) {
	m_Shininiess = shininiess;
}

Pine::Shader* Pine::Material::GetShader() {
	return m_Shader;
}

void Pine::Material::SetShader(Shader* shader) {
	m_Shader = shader;
}

bool Pine::Material::LoadFromFile()
{
	const auto j = Pine::Serialization::LoadJSONFromFile(m_FilePath.string());

	try {

		m_Shader = dynamic_cast<Shader*>(Serialization::LoadAsset(j, "Shader"));

		m_DiffuseColor = Serialization::LoadVec3(j, "DiffuseColor");
		m_SpecularColor = Serialization::LoadVec3(j, "SpecularColor");
		m_AmbientColor = Serialization::LoadVec3(j, "AmbientColor");

		m_Diffuse = dynamic_cast<Texture*>(Serialization::LoadAsset(j, "Diffuse"));
		m_Specular = dynamic_cast<Texture*>(Serialization::LoadAsset(j, "Specular"));

	}
	catch (...) {

	}

	return true;
}

bool Pine::Material::SaveToFile()
{
	return false;
}

void Pine::Material::Dispose()
{

}
