#pragma once
#include "../IComponent/IComponent.hpp"
#include <glm/mat4x4.hpp>

namespace Pine
{

	class Camera : public IComponent
	{
	private:
		float m_NearPlane = 1.f;
		float m_FarPlane = 300.f;
		float m_FieldOfView = 70.f;

		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;

		void BuildProjectionMatrix( );
		void BuildViewMatrix( );

		float m_AspectRatio = 0.0f;
	public:
		Camera( );

		float GetNearPlane( ) const;
		float GetFarPlane( ) const;
		float GetFieldOfView( ) const;

		void SetNearPlane( float value );
		void SetFarPlane( float value );
		void SetFieldOfView( float value );

		glm::mat4& GetProjectionMatrix( );
		glm::mat4& GetViewMatrix( );

		void OnSetup( ) override;
		void OnRender( ) override;
		void OnUpdate( float deltaTime ) override;

		void SaveToJson( nlohmann::json& j ) override;
		void LoadFromJson( nlohmann::json& j ) override;
	};

}