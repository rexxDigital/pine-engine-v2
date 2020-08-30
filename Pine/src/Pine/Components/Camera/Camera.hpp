#pragma once
#include "../IComponent/IComponent.hpp"
#include <glm/mat4x4.hpp>

namespace Pine {

	class Camera : public IComponent {
	private:
		float m_NearPlane = 0.1f;
		float m_FarPlane = 100.f;
		float m_FieldOfView = 70.f;

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;

		void BuildProjectionMatrix();
		void BuildViewMatrix();

		float m_AspectRatio = 0.0f;
	public:
		Camera();

		float GetNearPlane() const;
		float GetFarPlane() const;
		float GetFieldOfView() const;

		void SetNearPlane(float value);
		void SetFarPlane(float value);
		void SetFieldOfViewPlane(float value);

		const glm::mat4& GetProjectionMatrix() const;
		const glm::mat4& GetViewMatrix() const;

		void OnSetup() override;
		void OnRender() override;
		void OnUpdate(float deltaTime) override;
		
	};

}