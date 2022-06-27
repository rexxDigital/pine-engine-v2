#pragma once
#include "../IComponent/IComponent.hpp"
#include <glm/mat4x4.hpp>

namespace Pine
{

    enum CameraType
    {
        Perspective,
        Orthographic
    };

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

        CameraType m_CameraType = CameraType::Perspective;
	public:
		Camera( );

		float GetNearPlane( ) const;
		float GetFarPlane( ) const;
		float GetFieldOfView( ) const;
        CameraType GetCameraType( ) const;

		void SetNearPlane( float value );
		void SetFarPlane( float value );
		void SetFieldOfView( float value );
        void SetCameraType( CameraType type );

		glm::mat4& GetProjectionMatrix( );
		glm::mat4& GetViewMatrix( );

		void OnSetup( ) override;
		void OnRender( ) override;
		void OnUpdate( float deltaTime ) override;

		void SaveToJson( nlohmann::json& j ) override;
		void LoadFromJson( nlohmann::json& j ) override;
	};

}