#include "Windows.hpp"

#include <Pine/Pine.hpp>
#include <Pine/Entitylist/EntityList.hpp>

void Editor::Gui::Windows::RenderEntitylist( ) { 

	if ( !ShowEntitylist )
		return;

	ImGui::Begin( "Entity list", &ShowEntitylist );

	

	ImGui::End( );

}
