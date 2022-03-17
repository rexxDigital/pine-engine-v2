#include "MainMenuBar.hpp"

#include "../Utility/HotkeyManager/HotkeyManager.hpp"


#include "../Windows/Windows.hpp"
#include "../../ProjectManager/ProjectManager.hpp"

#include <imgui/imgui.h>

#include "Pine/Assets/Assets.hpp"
#include "Pine/Core/Log/Log.hpp"

void Editor::Gui::MainMenuBar::Render( )
{
	bool openAboutPage = false;
	bool openSettingsPage = false;

	if ( HotkeyManager::GetHotkeyPressed( Hotkeys::SaveHotkey ) )
	{
		ProjectManager::Save( );
	}

	if ( HotkeyManager::GetHotkeyPressed( Hotkeys::RefreshAssetsHotkey ) )
	{
		ProjectManager::ReloadProjectAssets( );
	}

	if ( HotkeyManager::GetHotkeyPressed( Hotkeys::RefreshEngineAssets ) )
	{
		Pine::Log->Message( "Loading engine assets..." );

		Pine::Assets->LoadFromDirectory( "Assets\\Engine\\Shaders", true );
		Pine::Assets->LoadFromDirectory( "Assets\\Engine", true );
	}

	if ( ImGui::BeginMainMenuBar( ) )
	{
		if ( ImGui::BeginMenu( "File" ) )
		{
			if ( ImGui::MenuItem( "New Level" ) )
			{

			}

			if ( ImGui::MenuItem( "Save", "CTRL+S" ) )
			{
				ProjectManager::Save( );
			}

			if ( ImGui::MenuItem( "Level Settings" ) )
			{
				Windows::ShowLevelSettings = !Windows::ShowLevelSettings;
			}

			if ( ImGui::MenuItem( "Settings" ) )
			{
				openSettingsPage = true;
			}

			ImGui::Separator( );

			if ( ImGui::MenuItem( "Exit" ) )
			{

			}

			ImGui::EndMenu( );
		}

		if ( ImGui::BeginMenu( "Edit" ) )
		{

			ImGui::EndMenu( );
		}

		if ( ImGui::BeginMenu( "Assets" ) )
		{

			if ( ImGui::MenuItem( "Refresh", "F5" ) )
			{
				ProjectManager::ReloadProjectAssets( );
			}

			if ( ImGui::MenuItem( "Import..." ) )
			{

			}

			if ( ImGui::MenuItem( "Reload Engine Assets", "CTRL+F5" ) )
			{
				Pine::Log->Message( "Loading engine assets..." );

				Pine::Assets->LoadFromDirectory( "Assets\\Engine\\Shaders", true );
				Pine::Assets->LoadFromDirectory( "Assets\\Engine", true );
			}

			ImGui::EndMenu( );
		}

		if ( ImGui::BeginMenu( "Windows" ) )
		{
			ImGui::MenuItem( "Asset Browser", nullptr, &Windows::ShowAssetBrowser );
			ImGui::MenuItem( "Entity List", nullptr, &Windows::ShowEntitylist );
			ImGui::MenuItem( "Properties", nullptr, &Windows::ShowProperties );
			ImGui::MenuItem( "Game Viewport", nullptr, &Windows::ShowGameViewport );
			ImGui::MenuItem( "Level Viewport", nullptr, &Windows::ShowLevelViewport );
			ImGui::MenuItem( "Level Settings", nullptr, &Windows::ShowLevelSettings );
			ImGui::MenuItem( "Console", nullptr, &Windows::ShowConsole );

			if ( ImGui::BeginMenu( "Debug" ) )
			{
				ImGui::MenuItem( "Rendering Context", nullptr, &Windows::ShowRenderingContext );
				ImGui::MenuItem( "Components System", nullptr, &Windows::ShowComponentsSys );

				ImGui::EndMenu( );
			}

			ImGui::EndMenu( );
		}

		if ( ImGui::BeginMenu( "Help" ) )
		{
			if ( ImGui::MenuItem( "About" ) )
			{
				openAboutPage = true;
			}

			ImGui::EndMenu( );
		}


		ImGui::EndMainMenuBar( );
	}

	if ( openAboutPage )
	{
		ImGui::OpenPopup( "About" );
		ImGui::SetNextWindowSize( ImVec2( 700, 400 ) );
	}

	if ( ImGui::BeginPopupModal( "About" ) )
	{
		ImGui::Text( "Pine: A OpenGL 2D/3D Game Engine" );
		ImGui::Spacing( );
		ImGui::Spacing( );
		ImGui::Spacing( );
		ImGui::Text( "Built via: " );
		ImGui::Text( "GLFW" );
		ImGui::Text( "GLEW" );
		ImGui::Text( "assimp" );
		ImGui::Text( "glm" );
		ImGui::Text( "json by nlohmann" );
		ImGui::Text( "stb_image" );
		ImGui::Text( "Dear ImGui" );
		ImGui::Text( "Freetype" );
		ImGui::Text( "ReactPhysics3D" );

		ImGui::Spacing( );

		if ( ImGui::Button( "OK" ) )
		{
			ImGui::CloseCurrentPopup( );
		}
		
		ImGui::EndPopup( );
	}

	if ( openSettingsPage )
	{
		ImGui::OpenPopup( "Settings" );
	}

}
