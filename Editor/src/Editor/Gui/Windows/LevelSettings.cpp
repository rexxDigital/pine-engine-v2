#include "Windows.hpp"

using namespace Editor::Gui;

void Editor::Gui::Windows::RenderLevelSettings( )
{
	if ( !ShowLevelSettings )
		return;
	
	ImGui::Begin( "Level Settings", &ShowLevelSettings, 0 );


	
	ImGui::End( );
}
