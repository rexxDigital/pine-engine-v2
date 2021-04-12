#include "MainMenuBar.hpp"

#include "../Windows/Windows.hpp"
#include "../../ProjectManager/ProjectManager.hpp"
#include "../Utility/HotkeyManager/HotkeyManager.hpp"

#include <imgui/imgui.h>

#include "Pine/Assets/Assets.hpp"
#include "Pine/Core/Log/Log.hpp"

PE_REGISTER_HOTKEY( SaveHotkey, GLFW_KEY_S, true, false );
PE_REGISTER_HOTKEY( RefreshAssetsHotkey, GLFW_KEY_F5, false, false );

void Editor::Gui::MainMenuBar::Render( ) {

	if ( HotkeyManager::GetHotkeyPressed( SaveHotkey ) )
	{
		ProjectManager::Save( );
	}
	
	if ( HotkeyManager::GetHotkeyPressed( RefreshAssetsHotkey ) )
	{
		ProjectManager::ReloadProjectAssets( );
	}
	
	if ( ImGui::BeginMainMenuBar( ) ) {
		if ( ImGui::BeginMenu( "File" ) ) {
			if ( ImGui::MenuItem( "New Level" ) ) {

			}
			
			if ( ImGui::MenuItem( "Save", "CTRL+S" ) )
			{
				ProjectManager::Save( );
			}

			if ( ImGui::MenuItem( "Level Settings" ) )
			{
				Windows::ShowLevelSettings = !Windows::ShowLevelSettings;
			}

			ImGui::Separator( );

			if ( ImGui::MenuItem( "Exit" ) )
			{

			}

			ImGui::EndMenu( );
		}

		if ( ImGui::BeginMenu( "Edit" ) ) {

			ImGui::EndMenu( );
		}

		if ( ImGui::BeginMenu( "Assets" ) ) {

			if ( ImGui::MenuItem( "Refresh", "F5" ) ) {
				ProjectManager::ReloadProjectAssets( );
			}

			if ( ImGui::MenuItem( "Import..." ) ) {

			}

			if ( ImGui::MenuItem( "Reload Engine Shaders" ) )
			{
				Pine::Log::Message( "Loading engine shaders..." );

				if ( Pine::Assets::LoadFromDirectory( "Assets\\Engine\\Shaders", true ) == 0 ) {
					Pine::Log::Fatal( "Failed to load engine shaders." );
				}
			}

			if ( ImGui::MenuItem( "Reload All Engine Assets" ) )
			{

			}
			
			ImGui::EndMenu( );
		}

		if ( ImGui::BeginMenu( "Windows" ) ) {

			ImGui::MenuItem( "Asset Browser", nullptr, &Windows::ShowAssetBrowser );
			ImGui::MenuItem( "Entity List", nullptr, &Windows::ShowEntitylist );
			ImGui::MenuItem( "Properties", nullptr, &Windows::ShowProperties );
			ImGui::MenuItem( "Game Viewport", nullptr, &Windows::ShowGameViewport );
			ImGui::MenuItem( "Level Viewport", nullptr, &Windows::ShowLevelViewport );
			ImGui::MenuItem( "Level Settings", nullptr, &Windows::ShowLevelSettings );

			if ( ImGui::BeginMenu( "Debug" ) ) {
				ImGui::MenuItem( "Rendering Context", nullptr, &Windows::ShowRenderingContext );

				ImGui::EndMenu( );
			}

			ImGui::EndMenu( );
		}
		ImGui::EndMainMenuBar( );
	}

}
