#include "Light.hpp"

#include "../../../ImGui/imgui.h"
#include "../../Core/Serialization/Serialization.hpp"

Pine::Light::Light( ) {
	m_ComponentType = EComponentType::Light;
}

void Pine::Light::SetLightColor( glm::vec3 lightColor ) {
	m_LightColor = lightColor;
}

const glm::vec3& Pine::Light::GetLightColor( ) const {
	return m_LightColor;
}

void Pine::Light::SetLightType( ELightType type ) {
	m_LightType = type;
}

Pine::ELightType Pine::Light::GetLightType( ) const {
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

void Pine::Light::OnSetup( ) {}

void Pine::Light::OnUpdate( float deltaTime ) {}

void Pine::Light::SaveToJson( nlohmann::json& j )
{
	Serialization::SaveVec3( j[ "light_color" ], m_LightColor );
	Serialization::SaveVec3( j[ "anno" ], m_Attenuation );
	j[ "light_type" ] = static_cast< int >( m_LightType );
}

void Pine::Light::LoadFromJson( nlohmann::json& j )
{
	m_LightColor = Serialization::LoadVec3( j, "light_color" );
	m_Attenuation = Serialization::LoadVec3( j, "anno" );
	m_LightType = static_cast< Pine::ELightType >( j[ "light_type" ].get<int>( ) );
}

Pine::IComponent* Pine::Light::Clone( )
{
	auto ret = new Pine::Light( );

	ret->SetLightColor( m_LightColor );
	ret->SetLightType( m_LightType );

	return ret;
}

