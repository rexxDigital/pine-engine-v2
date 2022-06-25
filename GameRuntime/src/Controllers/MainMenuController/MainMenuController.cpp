#include "MainMenuController.hpp"
#include <Pine/Entitylist/EntityList.hpp>
#include <Pine/Rendering/RenderManager/RenderManager.hpp>

namespace
{
    float EaseOutCubic(float x)
    {
        return x < 0.5 ? 4 * x * x * x : 1 - pow(-2 * x + 2, 3) / 2;
    }
}

void MainMenuController::OnSetup() {
    m_Chandiler = Pine::EntityList->FindEntity("Chandlier");
    m_ChandilerRotation = 0.f;

    m_Light = Pine::EntityList->FindEntity("Light");
    m_Light->GetComponent<Pine::Light>()->SetLightColor(glm::vec3(0.85f));
    m_LightTimer = 0.1;

    Pine::RenderManager->GetRenderingContext()->m_Camera = Pine::EntityList->FindEntity("Camera")->GetComponent<Pine::Camera>();
}

void MainMenuController::OnRender() {


}

void MainMenuController::OnUpdate(float deltaTime) {
    UpdateLight(deltaTime);
    UpdateChandiler(deltaTime);
}

void MainMenuController::UpdateChandiler(float deltaTime) {
    deltaTime = 0.002f;

    m_Chandiler->GetTransform()->Rotation.x = (20.f * EaseOutCubic(m_ChandilerRotation)) - 10.f;

    m_ChandilerRotation += m_ChandilerDirection ? deltaTime : -deltaTime;

    if (fabsf(m_ChandilerRotation) >= 1.f || m_ChandilerRotation < 0.f)
    {
        m_ChandilerDirection = !m_ChandilerDirection;
    }
}

void MainMenuController::UpdateLight(float deltaTime) {
    m_LightTimer -= deltaTime;

    if (0 >= m_LightTimer)
    {
        m_LightDirection = !m_LightDirection;
        m_LightTimer = 0.15f;
    }

    auto light = m_Light->GetComponent<Pine::Light>();
    float brightness = light->GetLightColor().x;

    if (m_LightDirection)
        brightness += 0.001f;
    else
        brightness -= 0.001f;

    light->SetLightColor(glm::vec3(brightness));
}

void MainMenuController::OnRenderUI( ) {

}
