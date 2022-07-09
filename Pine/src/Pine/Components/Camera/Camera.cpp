#include "Camera.hpp"
#include "../../Core/Window/Window.hpp"
#include "../../Entity/Entity.hpp"
#include "../Transform/Transform.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "../../../ImGui/imgui.h"
#include "../../Rendering/RenderManager/RenderManager.hpp"

void Pine::Camera::BuildProjectionMatrix( )
{
	const auto renderingContext = RenderManager->GetRenderingContext( );

	m_AspectRatio = static_cast< float >( renderingContext->m_Width ) / static_cast< float >( renderingContext->m_Height );

    if (m_CameraType == Pine::CameraType::Perspective)
	    m_ProjectionMatrix = glm::perspective( glm::radians( m_FieldOfView ), m_AspectRatio, m_NearPlane, m_FarPlane );
    if (m_CameraType == Pine::CameraType::Orthographic)
        m_ProjectionMatrix = glm::ortho( -10.0f, 10.0f, -10.0f, 10.0f, m_NearPlane, m_FarPlane );
}

void Pine::Camera::BuildViewMatrix( )
{
	const auto transform = m_Parent->GetTransform( );
    const auto rotation = glm::normalize( transform->GetRotationSum( ) );

    const auto direction = rotation * glm::vec3(0.f, 0.f, -1.0f);
    const auto up = rotation * glm::vec3(0.f, 1.f, 0.0f);

    const auto position = transform->GetPositionSum( );

	m_ViewMatrix = glm::lookAt( position, position + direction, up );
}

Pine::Camera::Camera( )
{
	m_ComponentType = ComponentType::Camera;
}

float Pine::Camera::GetNearPlane( ) const
{
	return m_NearPlane;
}

float Pine::Camera::GetFarPlane( ) const
{
	return m_FarPlane;
}

float Pine::Camera::GetFieldOfView( ) const
{
	return m_FieldOfView;
}

void Pine::Camera::SetNearPlane( const float value )
{
	m_NearPlane = value;
	BuildProjectionMatrix( );
}

void Pine::Camera::SetFarPlane( const float value )
{
	m_FarPlane = value;
}

void Pine::Camera::SetFieldOfView( const float value )
{
	m_FieldOfView = value;
	BuildProjectionMatrix( );
}

glm::mat4& Pine::Camera::GetProjectionMatrix( )
{
	return m_ProjectionMatrix;
}

glm::mat4& Pine::Camera::GetViewMatrix( )
{
	return m_ViewMatrix;
}

void Pine::Camera::OnSetup( )
{
	BuildProjectionMatrix( );
}

void Pine::Camera::OnRender( )
{
	BuildProjectionMatrix( );
	BuildViewMatrix( );
}

void Pine::Camera::OnUpdate( float deltaTime )
{

}

void Pine::Camera::SaveToJson( nlohmann::json& j )
{
	j[ "near_plane" ] = m_NearPlane;
	j[ "far_plane" ] = m_FarPlane;
	j[ "fov" ] = m_FieldOfView;
	j[ "camType" ] = m_CameraType;
}

void Pine::Camera::LoadFromJson( nlohmann::json& j )
{
	m_NearPlane = j[ "near_plane" ];
	m_FarPlane = j[ "far_plane" ];
	m_FieldOfView = j[ "fov" ];

    if (j.contains("camType"))
        m_CameraType = static_cast<Pine::CameraType>(j["camType"].get<int>());
}

Pine::CameraType Pine::Camera::GetCameraType() const {
    return m_CameraType;
}

void Pine::Camera::SetCameraType(Pine::CameraType type) {
    m_CameraType = type;
}
