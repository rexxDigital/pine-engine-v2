#include "Windows.hpp"

void Editor::Gui::Windows::RenderAssetBrowser( ) {

	if ( !ShowAssetBrowser ) {
		return;
	}

	ImGui::Begin( "Asset Browser", &ShowAssetBrowser, 0 );
	


	ImGui::End( );

}