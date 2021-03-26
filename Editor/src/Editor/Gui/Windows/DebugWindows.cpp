#include "Windows.hpp"
#include <Pine\Rendering\RenderManager\RenderManager.hpp>
#include <Pine\Entity\Entity.hpp>

using namespace Editor::Gui;

namespace {

	void RenderRenderingContext( ) {
		if ( ImGui::Begin( "Rendering Context", &Windows::ShowRenderingContext, 0 ) ) {
			auto context = Pine::RenderManager::GetRenderingContext( );

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

}

void Windows::RenderDebugWindows( ) {

	if ( Windows::ShowRenderingContext ) {
		RenderRenderingContext( );
	}

}