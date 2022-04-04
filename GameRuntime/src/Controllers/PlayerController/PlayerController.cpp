#include "PlayerController.hpp"

#include <glm/ext/matrix_projection.hpp>

#include "Pine/Pine.hpp"
#include "Pine/Entity/Entity.hpp"
#include "Pine/Components/RigidBody/RigidBody.hpp"

#include <glfw/glfw3.h>

void PlayerController::OnSetup( )
{
    Pine::Log->Message( "PlayerController::OnSetup( )" );

    // This would eventually be configured within the editor, instead of being created through code.

    m_Pitch = Pine::Input->CreateBinding("Player Pitch");
    m_Pitch->AddAxisBinding(Pine::Axis::MouseY, 0.1f );

    m_Yaw = Pine::Input->CreateBinding("Player Yaw");
    m_Yaw->AddAxisBinding(Pine::Axis::MouseX, 0.1f );

    m_Forward = Pine::Input->CreateBinding( "Player Forward" );
    m_Forward->AddKeyboardBinding( GLFW_KEY_W, 1.f );
    m_Forward->AddKeyboardBinding( GLFW_KEY_S, -1.f );

    m_Sideways = Pine::Input->CreateBinding( "Player Sideways" );
    m_Sideways->AddKeyboardBinding( GLFW_KEY_D, 1.f );
    m_Sideways->AddKeyboardBinding( GLFW_KEY_A, -1.f );

    Pine::Input->SetCursorAutoCenter( true );
    Pine::Input->SetCursorVisible( false );

    // "Cache" the camera entity
    m_CameraEntity = m_Parent->GetChildren()[0];
}

void PlayerController::OnRender( )
{
    const float speed = 0.05f;

    auto transform = m_Parent->GetTransform();
    auto camTransform = m_CameraEntity->GetTransform();

    // Handle rotation, however rotate only the camera for the pitch axis
    camTransform->Rotation.x += m_Pitch->Value();
    transform->Rotation.y += m_Yaw->Value();

    // Clamp and normalize angles
    camTransform->Rotation.x = std::clamp(camTransform->Rotation.x, -90.f, 90.f);

    while ( transform->Rotation.y >= 180.f )
        transform->Rotation.y -= 360.f;
    while ( transform->Rotation.y <= -180.f )
        transform->Rotation.y += 360.f;

    // Handle movement
    transform->Position += transform->GetForward() * speed * m_Forward->Value();
    transform->Position += transform->GetRight() * speed * m_Sideways->Value();
}

void PlayerController::OnUpdate( float deltaTime )
{

}