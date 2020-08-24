#pragma once
#include "../IAsset/IAsset.hpp"
#include "../Shader/Shader.hpp"
#include "../Texture/Texture.hpp"

#include <glm/vec3.hpp>

namespace Pine
{

	class Material : public IAsset
	{
	private:
		glm::vec3 m_DiffuseColor = glm::vec3(1.f, 1.f, 1.f);
		glm::vec3 m_SpecularColor = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3 m_AmbientColor = glm::vec3(0.5f, 0.5f, 0.5f);

		float m_Shininiess = 16.f;

		Texture* m_Diffuse = nullptr;
		Texture* m_Specular = nullptr;

		Shader* m_Shader = nullptr;
	public:
		Material();

		glm::vec3& DiffuseColor();
		glm::vec3& SpecularColor();
		glm::vec3& AmbientColor();
		
		Texture* GetDiffuse() const;
		Texture* GetSpecular() const;

		void SetDiffuse(Texture* texture);
		void SetSpecular(Texture* texture);

		float GetShininiess() const;
		void SetShininiess(float shininiess);
		
		Shader* GetShader();
		void SetShader(Shader* shader);

		bool LoadFromFile() override;
		bool SaveToFile() override;
		void Dispose() override;
	};

}