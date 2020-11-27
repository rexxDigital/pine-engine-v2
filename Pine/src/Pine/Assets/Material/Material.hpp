#pragma once
#include "../IAsset/IAsset.hpp"
#include "../Shader/Shader.hpp"
#include "../Texture2D/Texture2D.hpp"

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

		Texture2D* m_Diffuse = nullptr;
		Texture2D* m_Specular = nullptr;

		Shader* m_Shader = nullptr;

		bool m_IsGenerated = false;

		float m_TextureScale = 1.f;
	public:
		Material();

		glm::vec3& DiffuseColor();
		glm::vec3& SpecularColor();
		glm::vec3& AmbientColor();
		
		Texture2D* GetDiffuse() const;
		Texture2D* GetSpecular() const;

		void SetDiffuse(Texture2D* texture);
		void SetSpecular(Texture2D* texture);

		float GetTextureScale( ) const;
		void SetTextureScale( float scale );

		float GetShininiess() const;
		void SetShininiess(float shininiess);
		
		Shader* GetShader();
		void SetShader(Shader* shader);

		bool IsGenerated() const;
		void SetGenerated(bool generated);
		
		bool LoadFromFile() override;
		bool SaveToFile() override;
		void Dispose() override;
	};

}