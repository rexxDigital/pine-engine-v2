#include "Windows.hpp"
#include <Pine/Rendering/RenderManager/RenderManager.hpp>
#include <Pine/Entity/Entity.hpp>
#include <Pine/Components/Components.hpp>
#include <Pine/PhysicsManager/PhysicsManager.hpp>

#include "../Widgets/Widgets.hpp"

#include "Pine/RuntimeLoader/RuntimeLoader.hpp"
#include "Editor/Gui/Utility/EntityPickSystem/EntityPickSystem.hpp"

using namespace Editor::Gui;

namespace {

	void RenderRenderingContext( ) {

		if ( ImGui::Begin( "Rendering Context", &Windows::ShowRenderingContext, 0 ) ) {
			auto context = Pine::RenderManager->GetRenderingContext( );

			if ( !context ) {
				ImGui::Text( "The rendering context is a nullptr." );
			}
			else {
				ImGui::Text( "Camera entity: %s", context->m_Camera ? context->m_Camera->GetParent( )->GetName( ).c_str( ) : "None [!]" );

				if ( !context->m_Camera && ImGui::IsItemHovered( ) )
					ImGui::SetTooltip( "There is no active camera, as a result the\nscene will never get rendered." );

				ImGui::Text( "Frame buffer: %s", context->m_FrameBuffer ? "Yes" : "No" );
				ImGui::Text( "Width: %d", context->m_Width );
				ImGui::Text( "Height: %d", context->m_Height );
				ImGui::Text( "Is 3D: %s", context->m_Is3D ? "Yes" : "No" );

				ImGui::Text( "Draw calls: %d", context->m_DrawCalls );
				ImGui::Text( "Entity sort time: %f", context->m_EntitySortTime );

				if ( ImGui::IsItemHovered( ) )
					ImGui::SetTooltip( "The time it takes for the engine to process each entity to prepare for rendering." );

				ImGui::Text( "Entity render time: %f", context->m_EntityRenderTime );
				ImGui::Text( "Post processing render time: %f", context->m_PostProcessingTime );
			}
		}
		ImGui::End( );
	}

    void RenderComponentsSystem( )
    {
        if ( ImGui::Begin( "Rendering Context", &Windows::ShowRenderingContext, 0 ) ) {

            const auto avSize = ImGui::GetContentRegionAvail( );

            ImGui::Image( reinterpret_cast< ImTextureID >( Editor::Gui::Utility::EntityPickSystem::GetFrameBuffer( )->GetTextureId() ),
                          avSize, ImVec2( 0.f, 1.f ), ImVec2( 1.f, 0.f ) );


        }
        ImGui::End();
    }

    void RenderPhysicsProfiler( )
    {
        if ( ImGui::Begin( "Physics Profiler", &Windows::ShowPhysicsProfiler, 0 ) ) {

            ImGui::Text("Pre physics update: %f", Pine::PhysicsManager->GetPrePhysicsTime());
            ImGui::Text("Physics update: %f", Pine::PhysicsManager->GetPhysicsProcessTime());
            ImGui::Text("Post physics update: %f", Pine::PhysicsManager->GetPostPhysicsTime());

            const auto world = Pine::PhysicsManager->GetPhysicsWorld();

            ImGui::Text("Rigid bodies: %d", world->getNbRigidBodies());
            ImGui::Text("Collision bodies: %d", world->getNbCollisionBodies());
            ImGui::Text("Debug rendering: %d", world->getIsDebugRenderingEnabled());
        }
        ImGui::End();
    }

}

void Windows::RenderDebugWindows( ) {

	if ( Windows::ShowRenderingContext ) {
		RenderRenderingContext( );
	}

    if ( Windows::ShowComponentsSys ) {
        RenderComponentsSystem( );
    }

    if ( Windows::ShowPhysicsProfiler ) {
        RenderPhysicsProfiler( );
    }

}