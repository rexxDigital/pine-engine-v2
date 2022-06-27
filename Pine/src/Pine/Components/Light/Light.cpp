#include "Light.hpp"

#include "../../../ImGui/imgui.h"
#include "../../Core/Serialization/Serialization.hpp"

Pine::Light::Light( )
{
	m_ComponentType = ComponentType::Light;
}

void Pine::Light::SetLightColor( glm::vec3 lightColor )
{
	m_LightColor = lightColor;
}

const glm::vec3& Pine::Light::GetLightColor( ) const
{
	return m_LightColor;
}

void Pine::Light::SetLightType( LightType type )
{
	m_LightType = type;
}

Pine::LightType Pine::Light::GetLightType( ) const
{
	return m_LightType;
}

void Pine::Light::SetAttenuation( glm::vec3 vec )
{
	m_Attenuation = vec;
}

const glm::vec3& Pine::Light::GetAttenuation( ) const
{
	return m_Attenuation;
}

void Pine::Light::SetSpotlightAngle( float value )
{
	m_SpotLightAngle = value;
}

float Pine::Light::GetSpotlightAngle( ) const
{
	return m_SpotLightAngle;
}

void Pine::Light::SetSpotlightSmoothness( float value )
{
	m_SpotLightSmoothness = value;
}

float Pine::Light::GetSpotlightSmoothness( ) const
{
	return m_SpotLightSmoothness;
}

void Pine::Light::OnSetup( ) { }

void Pine::Light::OnUpdate( float deltaTime ) { }

void Pine::Light::SaveToJson( nlohmann::json& j )
{
	Serialization::SaveVec3( j[ "light_color" ], m_LightColor );
	Serialization::SaveVec3( j[ "anno" ], m_Attenuation );
	j[ "light_type" ] = m_LightType;
	j[ "angle" ] = m_SpotLightAngle;
	j[ "smoothness" ] = m_SpotLightSmoothness;
	j[ "castShadows" ] = m_CastShadows;
}

void Pine::Light::LoadFromJson( nlohmann::json& j )
{
	m_LightColor = Serialization::LoadVec3( j, "light_color" );
	m_Attenuation = Serialization::LoadVec3( j, "anno" );
	m_LightType = static_cast< Pine::LightType >( j[ "light_type" ].get<int>( ) );

	if ( j.contains( "angle" ) )
		m_SpotLightAngle = j[ "angle" ].get<float>( );
	if ( j.contains( "smoothness" ) )
		m_SpotLightSmoothness = j[ "smoothness" ].get<float>( );
    if ( j.contains( "castShadows" ) )
        m_CastShadows = j[ "castShadows" ].get<bool>( );
}

void Pine::Light::SetCastShadows( bool value ) {
    m_CastShadows = value;
}

bool Pine::Light::GetCastShadows( ) const {
    return m_CastShadows;
}

