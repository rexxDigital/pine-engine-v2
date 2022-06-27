#include "EntityPickSystem.hpp"
#include "Editor/Gui/Gui.hpp"
#include <Pine/Rendering/RenderManager/RenderManager.hpp>
#include <Pine/Rendering/Renderer3D/Renderer3D.hpp>
#include <Pine/OpenGL/FrameBuffer/FrameBuffer.hpp>
#include <Pine/Assets/Model/Model.hpp>
#include <Pine/Components/ModelRenderer/ModelRenderer.hpp>
#include <Pine/Entity/Entity.hpp>
#include <ImGui/imgui.h>
#include <Pine/Core/Log/Log.hpp>
#include <Pine/Entitylist/EntityList.hpp>

#include <GL/glew.h>

namespace
{
    Pine::FrameBuffer* m_FrameBuffer = nullptr;
    Pine::RenderingContext* m_RenderingContext = nullptr;

    Pine::Shader* m_PickShader = nullptr;

    glm::vec2 m_ViewportPosition;
    glm::vec2 m_ViewportSize;
}

void Editor::Gui::Utility::EntityPickSystem::Setup()
{
    m_FrameBuffer = new Pine::FrameBuffer();
    m_FrameBuffer->Create(1920, 1080, Pine::Buffers::TextureBuffer | Pine::Buffers::DepthBuffer);

    m_RenderingContext = new Pine::RenderingContext();
    m_RenderingContext->m_Width = 1920;
    m_RenderingContext->m_Height = 1080;
    m_RenderingContext->m_ClearColor = glm::vec4( 0.f, 0.f, 0.f, 1.f );
    m_RenderingContext->m_FrameBuffer = m_FrameBuffer;

    m_PickShader = Pine::Assets->GetAsset<Pine::Shader>("Assets/Editor/Shaders/PickShader.shr");
}

void Editor::Gui::Utility::EntityPickSystem::Dispose()
{
    if (m_FrameBuffer)
        m_FrameBuffer->Dispose();

    delete m_FrameBuffer;
}

void Editor::Gui::Utility::EntityPickSystem::Render()
{
    if (!m_RenderingContext || !m_PickShader)
    {
        // Setup() hasn't been called for some reason.

        return;
    }

    // Only run this if we're actually pressing the mouse button, saves performance.
    if (!ImGui::IsMouseClicked(ImGuiMouseButton_Left))
        return;
    if (!Globals::IsHoveringLevelView)
        return;
    if (Globals::IsUsingGizmo || Globals::IgnorePickInput)
        return;

    // Apply new rendering context
    const auto backupRenderingContext = Pine::RenderManager->GetRenderingContext();

    Pine::RenderManager->SetRenderingContext(m_RenderingContext);

    m_RenderingContext->m_Camera = backupRenderingContext->m_Camera; // Should be the level camera.

    // Render all entities with our own shader, calling OnRender on all components and whatnot should not be
    // necessary here since we are currently running post the current frame.
    const auto renderBatch = Pine::RenderManager->GetRenderingBatch();

    Pine::RenderManager->PrepareSceneRendering();
    Pine::Renderer3D->PrepareMeshRendering();

    Pine::UniformVariable* renderColorUniform = nullptr;

    // I do not like having to replicate what the render manager does here, since what if I update the render manager in the future
    // but this will do for now.
    for ( auto& renderItem: renderBatch )
    {
        for ( auto& mesh: renderItem.first->GetMeshList( ) )
        {
            Pine::Renderer3D->PrepareMesh(mesh, nullptr);

            for ( const auto modelRenderer: renderItem.second )
            {
                const auto entity = modelRenderer->GetParent( );

                Pine::Renderer3D->SetShader(m_PickShader);

                if (!renderColorUniform)
                    renderColorUniform = m_PickShader->GetUniformVariable("renderingColor");

                // Compute the "color" we'll render with, i.e. entity index to base 256.
                float colorValue[4];

                std::memset(colorValue, 0, sizeof(colorValue));

                int value = entity->GetEntityIndex();
                int pass = 0;
                while (value >= 0)
                {
                    // Could use the modulo operator here but since I need both the values this is better.
                    auto tmp = std::div(value, 255);

                    colorValue[pass] = static_cast<float>(tmp.rem) / 255.f;

                    if (tmp.quot == 0)
                        break;

                    value -= tmp.quot;
                    pass++;

                    if (pass >= 3)
                    {
                        // We cannot encode this properly since the entity index is too high, should hopefully never happen.
                        break;
                    }
                }

                renderColorUniform->LoadVector4(glm::vec4(colorValue[0], colorValue[1], colorValue[2], 1.f));

                Pine::Renderer3D->RenderMesh( entity->GetTransform( )->GetTransformationMatrix( ) );
            }
        }
    }

    Pine::RenderManager->FinishSceneRendering( );

    // Find out which pixel we're hovering over.
    auto mousePositionViewport = glm::vec2(ImGui::GetMousePos().x, ImGui::GetMousePos().y) - m_ViewportPosition;

    // Since our underlying viewport is in 1920x1080, but we down scale that to fit the viewport, we
    // now have to up scale our cursor coordinates back to 1920x1080
    mousePositionViewport *= glm::vec2(1920, 1080) / m_ViewportSize;

    // Flip Y axis
    mousePositionViewport.y = 1080 - mousePositionViewport.y;

    std::uint8_t mouseColorData[4];

    // We can now read the color of the pixel at mousePositionViewport
    glReadPixels(mousePositionViewport.x, mousePositionViewport.y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, &mouseColorData);

    // Translate the color data back to base 10
    int digit0 = 0; // static_cast<int>(mouseColorData[3] * powf(255, 0));
    int digit1 = static_cast<int>(mouseColorData[2] * powf(255, 2));
    int digit2 = static_cast<int>(mouseColorData[1] * powf(255, 1));
    int digit3 = static_cast<int>(mouseColorData[0] * powf(255, 0));

    const int entityIndex = digit0 + digit1 + digit2 + digit3;

    Pine::Log->Message(std::to_string(entityIndex));

    if (entityIndex == 0)
    {
        Globals::SelectedEntityPtrs.clear();
    }
    else
    {
        const auto entities = Pine::EntityList->GetEntities();

        for (auto entity : entities)
        {
            if (entity->GetEntityIndex() == entityIndex)
            {
                Globals::SelectedEntityPtrs.clear();
                Globals::SelectedEntityPtrs.push_back(entity);
                break;
            }
        }
    }

    // Restore rendering context
    Pine::RenderManager->SetRenderingContext(backupRenderingContext);
}

Pine::FrameBuffer *Editor::Gui::Utility::EntityPickSystem::GetFrameBuffer()
{
    return m_FrameBuffer;
}

void Editor::Gui::Utility::EntityPickSystem::SetViewportPosition(float x, float y)
{
    m_ViewportPosition = glm::vec2(x, y);
}

void Editor::Gui::Utility::EntityPickSystem::SetViewportSize(float x, float y)
{
    m_ViewportSize = glm::vec2(x, y);
}
