#include "PlayerController.hpp"

#include <glm/ext/matrix_projection.hpp>

#include "Pine/Pine.hpp"
#include "Pine/Entity/Entity.hpp"
#include "Pine/Components/RigidBody/RigidBody.hpp"
#include "Pine/Entitylist/EntityList.hpp"

#include <GLFW/glfw3.h>


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
    m_CameraEntity = Pine::EntityList->FindEntity("Player Camera");
    m_HeadEntity = m_Parent->GetChildren()[0];
}

void PlayerController::OnRender( )
{
    const float speed = 0.05f;

    auto transform = m_Parent->GetTransform();
    auto camTransform = m_CameraEntity->GetTransform();

    m_Angle += glm::vec3( m_Pitch->Value( ), m_Yaw->Value( ), 0.f );

    // Handle rotation, however rotate only the camera for the pitch axis
    camTransform->SetEulerAngles(glm::vec3(m_Angle.x, m_Angle.y, 0.f));
    transform->SetEulerAngles(glm::vec3(0.f, m_Angle.y, 0.f));

    camTransform->Position = m_HeadEntity->GetTransform()->GetPositionSum();

    // Handle movement
    transform->Position += transform->GetForward() * speed * m_Forward->Value();
    transform->Position += transform->GetRight() * speed * m_Sideways->Value();
}

void PlayerController::OnUpdate( float deltaTime )
{

}