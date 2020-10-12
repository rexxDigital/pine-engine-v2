#include "MainMenuBar.hpp"

#include "../Windows/Windows.hpp"

#include <imgui/imgui.h>

void Editor::Gui::MainMenuBar::Render( ) {

	if ( ImGui::BeginMainMenuBar( ) ) {

		if ( ImGui::BeginMenu( "File" ) ) {

			ImGui::EndMenu( );
		}

		if ( ImGui::BeginMenu( "Edit" ) ) {

			ImGui::EndMenu( );
		}

		if ( ImGui::BeginMenu( "Assets" ) ) {

			ImGui::EndMenu( );
		}

		if ( ImGui::BeginMenu( "Windows" ) ) {

			ImGui::MenuItem( "Asset Browser", nullptr, &Windows::ShowAssetBrowser );
			ImGui::MenuItem( "Entity List", nullptr, &Windows::ShowEntitylist );
			ImGui::MenuItem( "Properties", nullptr, &Windows::ShowProperties );
			ImGui::MenuItem( "Game Viewport", nullptr, &Windows::ShowGameViewport );
			ImGui::MenuItem( "Level Viewport", nullptr, &Windows::ShowLevelViewport );

			ImGui::EndMenu( );
		}

		
		ImGui::EndMainMenuBar( );
	}

}
