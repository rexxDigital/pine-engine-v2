#pragma once
#include "../IComponent/IComponent.hpp"
#include <glm/vec3.hpp>

namespace Pine {

	enum ELightType {
		Normal
	};

	class Light : public IComponent {
	private:
		glm::vec3 m_LightColor = glm::vec3(1.f, 1.f, 1.f);
	public:
		Light();

		void SetLightColor(glm::vec3 lightColor);
		const glm::vec3& GetLightColor() const;

		virtual void OnSetup() override;
		virtual void OnUpdate(float deltaTime) override;
		void OnRenderGui() override;
	};

}