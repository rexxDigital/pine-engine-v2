#include "Windows.hpp"
#include <Pine/Rendering/RenderManager/RenderManager.hpp>
#include <Pine/Entity/Entity.hpp>
#include <Pine/Components/Components.hpp>
#include <Pine/PhysicsManager/PhysicsManager.hpp>

#include "../Widgets/Widgets.hpp"

#include "Pine/RuntimeLoader/RuntimeLoader.hpp"
#include "../Gui.hpp"
#include <Pine/Pine.hpp>

using namespace Editor::Gui;

void Windows::RenderProfiler( )
{

    if (ImGui::Begin("Profiler", &Windows::ShowProfiler))
    {
        // Some general data used everywhere
        const auto frameTime = Pine::GetFrameTime();
        const auto frameProcessTime = Pine::GetFrameProcessTimeInMs();

        auto renderingContext = Pine::RenderManager->GetRenderingContext();

        ImGui::Columns(3, nullptr, false);

        // General

        ImGui::PushFont(Fonts::BoldFont);
        ImGui::Text("General");
        ImGui::PopFont();

        ImGui::Text("Frame Time: %.4fs (%d FPS)", frameTime, Pine::GetFPS());
        ImGui::Text("Update Time: %.4fs (%d UPS)", Pine::GetUpdateTime(), static_cast<int>(1.f / Pine::GetUpdateTime()));
        ImGui::Text("Frame Process Time: %.4f ms (%.4fs)", frameProcessTime, frameProcessTime / 1000.f );

        if (renderingContext)
        {

        }

        ImGui::NextColumn();

        // Rendering

        ImGui::PushFont(Fonts::BoldFont);
        ImGui::Text("Rendering");
        ImGui::PopFont();

        if (renderingContext)
        {
            ImGui::Text( "Total rendering time: %.4f ms", renderingContext->m_TotalRenderTime );

            ImGui::Text("Entity update time: %.4f ms (%d%%)", renderingContext->m_EntityUpdateTime, static_cast<int>( renderingContext->m_EntityUpdateTime / renderingContext->m_TotalRenderTime * 100.f ));

            ImGui::Text( "Entity sort time: %.4f ms (%d%%)", renderingContext->m_EntitySortTime, static_cast<int>( renderingContext->m_EntitySortTime / renderingContext->m_TotalRenderTime * 100.f ) );

            if ( ImGui::IsItemHovered( ) )
                ImGui::SetTooltip( "The time it takes for the engine to process each entity to prepare for rendering." );

            ImGui::Text( "Entity render time: %.4f ms (%d%%)", renderingContext->m_EntityRenderTime, static_cast<int>( renderingContext->m_EntityRenderTime / renderingContext->m_TotalRenderTime * 100.f ) );
            ImGui::Text( "Post processing render time: %.4f ms (%d%%)", renderingContext->m_PostProcessingTime, static_cast<int>( renderingContext->m_PostProcessingTime / renderingContext->m_TotalRenderTime * 100.f ) );

            ImGui::Spacing();
            ImGui::Spacing();
            ImGui::Spacing();

            ImGui::Text("Entity update time per component");

            ImGui::BeginChild("##EntityUpdate", ImVec2(-1.f, 200.f), false);

            for (int i = 0; i < Pine::Components->GetComponentTypeCount();i++)
            {
                ImGui::Text("%s: %.4f ms", Pine::Components->GetComponentTypeName(static_cast<Pine::ComponentType>(i)), renderingContext->m_ComponentUpdateTime[i]);
            }

            ImGui::EndChild();
        }

        ImGui::NextColumn();

        // Physics

        ImGui::PushFont(Fonts::BoldFont);
        ImGui::Text("Physics");
        ImGui::PopFont();

        const auto totalPhysTime = Pine::PhysicsManager->GetPhysicsTotalProcessTime( );

        ImGui::Text( "Total physics process time: %.4f ms", totalPhysTime );

        ImGui::Text("Physics prepare time: %.4f ms (%d%%)", Pine::PhysicsManager->GetPrePhysicsTime( ), static_cast<int>( Pine::PhysicsManager->GetPrePhysicsTime( ) / totalPhysTime * 100.f ));

        ImGui::Text( "Physics process time: %.4f ms (%d%%)", Pine::PhysicsManager->GetPhysicsProcessTime( ), static_cast<int>( Pine::PhysicsManager->GetPhysicsProcessTime( ) / totalPhysTime * 100.f ) );

        ImGui::Text( "Physics finish time: %.4f ms (%d%%)", Pine::PhysicsManager->GetPostPhysicsTime( ), static_cast<int>( Pine::PhysicsManager->GetPostPhysicsTime( ) / totalPhysTime * 100.f ) );
        
        ImGui::Columns(1);

    }
    ImGui::End();

}