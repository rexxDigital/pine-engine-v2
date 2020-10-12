#include "Windows.hpp"

void Editor::Gui::Windows::RenderProperties( ) {

	if ( !ShowProperties ) {
		return;
	}

	ImGui::Begin( "Properties", &ShowProperties, 0 );

	ImGui::End( );

}