#include "PlayerController.hpp"
#include <iostream>

#include "Pine/Entity/Entity.hpp"

void PlayerController::OnSetup( )
{
	m_Pitch = Pine::Input->CreateBinding( "Pitch" );
	m_Yaw = Pine::Input->CreateBinding( "Yaw" );

	m_Pitch->AddAxisBinding( Pine::Axis::MouseY, 0.1f );
	m_Yaw->AddAxisBinding( Pine::Axis::MouseX, 0.1f );
}

void PlayerController::OnRender( )
{
	const auto transform = GetParent( )->GetTransform( );

	transform->Rotation.x += m_Pitch->Value( );
	transform->Rotation.y += m_Yaw->Value( );
	

}

void PlayerController::OnUpdate( float deltaTime )
{

}
