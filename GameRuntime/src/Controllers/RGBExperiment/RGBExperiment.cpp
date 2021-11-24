#include "RGBExperiment.hpp"

#include <glfw/glfw3.h>

#include "Pine/Components/Light/Light.hpp"
#include "Pine/Entitylist/EntityList.hpp"

void RGBExperiment::OnSetup( )
{
	m_R = Pine::EntityList->FindEntity( "R" );
	m_G = Pine::EntityList->FindEntity( "G" );
	m_B = Pine::EntityList->FindEntity( "B" );
}

void RGBExperiment::OnUpdate( const float deltaTime )
{
	if ( m_LightMovement )
	{
		m_MovementTime += deltaTime;

		m_R->GetTransform( )->Position.x = std::fabsf( std::sin( m_MovementTime / 50.f ) ) * 15.f;
		m_B->GetTransform( )->Position.x = std::fabsf( std::sin( m_MovementTime / 50.f ) ) * -15.f;
	}

	if ( Pine::Input->IsKeyReleased( GLFW_KEY_W ) )
	{
		m_LightMovement = !m_LightMovement;
	}
}