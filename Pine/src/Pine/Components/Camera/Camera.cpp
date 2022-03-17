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
	m_ProjectionMatrix = glm::perspective( glm::radians( m_FieldOfView ), m_AspectRatio, m_NearPlane, m_FarPlane );
}

void Pine::Camera::BuildViewMatrix( )
{
	const auto transform = m_Parent->GetTransform( );
    const auto rotation = transform->GetRotationSum( );

	const float verticalAngle = glm::radians( rotation.x );
	const float horizontalAngle = glm::radians( rotation.y );

	const glm::vec3 direction(
		std::cos( verticalAngle ) * std::sin( horizontalAngle ),
		std::sin( verticalAngle ),
		std::cos( verticalAngle ) * std::cos( horizontalAngle )
	);

	const glm::vec3 right = glm::vec3(
		std::sin( horizontalAngle - 3.14159265359f / 2.0f ),
		0,
		std::cos( horizontalAngle - 3.14159265359f / 2.0f )
	);

	const glm::vec3 up = glm::cross( right, direction );

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
}

void Pine::Camera::LoadFromJson( nlohmann::json& j )
{
	m_NearPlane = j[ "near_plane" ];
	m_FarPlane = j[ "far_plane" ];
	m_FieldOfView = j[ "fov" ];
}
