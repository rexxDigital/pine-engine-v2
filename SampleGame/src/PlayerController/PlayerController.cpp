#include "PlayerController.hpp"

#include "Pine/Entity/Entity.hpp"

void PlayerController::OnSetup( )
{

	m_LookPitch = Pine::Input::CreateBinding( "Look pitch" );
	m_LookYaw = Pine::Input::CreateBinding( "Look yaw" );

	m_LookPitch->AddAxisBinding( Pine::Input::Axis::MouseX, .1f );
	m_LookYaw->AddAxisBinding( Pine::Input::Axis::MouseY, .1f );

}

void PlayerController::OnUpdate( float deltaTime )
{

}

void PlayerController::OnRender( )
{
	auto transform = GetParent( )->GetTransform( );

	transform->Rotation.y += m_LookPitch->Value( );
	transform->Rotation.x += m_LookYaw->Value( );



}
