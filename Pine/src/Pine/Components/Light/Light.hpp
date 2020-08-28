#pragma once
#include "../IComponent/IComponent.hpp"
#include <glm/vec3.hpp>

namespace Pine {

	enum ELightType {
		Directional,
		SpotLight
	};

	class Light : public IComponent {
	private:
		glm::vec3 m_LightColor = glm::vec3(1.f, 1.f, 1.f);
		ELightType m_LightType = ELightType::Directional;
	public:
		Light();

		void SetLightColor(glm::vec3 lightColor);
		const glm::vec3& GetLightColor() const;

		void SetLightType(ELightType type);
		ELightType GetLightType() const;

		void OnSetup() override;
		void OnUpdate(float deltaTime) override;
		void OnRenderGui() override;
	};

}