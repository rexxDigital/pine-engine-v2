#include "Windows.hpp"
#include <Pine\Rendering\RenderManager\RenderManager.hpp>
#include <Pine\Entity\Entity.hpp>

#include "../Widgets/Widgets.hpp"
#include "../Gui.hpp"
#include "Editor/ProjectManager/ProjectManager.hpp"
#include "Pine/PhysicsManager/PhysicsManager.hpp"

#include "Pine/RuntimeLoader/RuntimeLoader.hpp"
#include "Editor/RuntimeManager/RuntimeManager.hpp"
#include "Pine/GameManager/GameManager.hpp"

using namespace Editor::Gui;

void Windows::RenderSettings( )
{
	bool open = false;

	if ( ImGui::IsPopupOpen( "Settings" ) )
		open = true;

	if ( ImGui::BeginPopupModal( "Settings", &open ) )
	{
		if ( !open ) // User clicked "X"
		{
			ImGui::CloseCurrentPopup( );
		}

		if ( ImGui::BeginTabBar( "SettingsTabs" ) )
		{
			if ( ImGui::BeginTabItem( "Engine Settings" ) )
			{
				ImGui::PushFont( Gui::Fonts::BoldFont );
				ImGui::Text( "Runtime Library" );
				ImGui::PopFont( );

				ImGui::Text( "Runtime library target file: %s", "GameRuntime.dll" );
				ImGui::Text( "Runtime library loaded: %s", Editor::RuntimeManager::GetProjectRuntimeLibrary( ) != nullptr ? "Yes" : "No" );
                ImGui::Text( "Runtime library source path: %s", Editor::RuntimeManager::GetRuntimeSourcePath( ).c_str( ) );
                ImGui::Text( "Runtime library compile available: %s", Editor::RuntimeManager::GetRuntimeCompileAvailable( ) ? "Yes" : "No" );

                bool autoCompile = Editor::RuntimeManager::GetRuntimeAutoCompile( );

                if ( ImGui::Checkbox( "Auto Compile", &autoCompile ) )
                {
                    Editor::RuntimeManager::SetRuntimeAutoCompile( autoCompile );
                }

                if ( ImGui::Button( "Compile", ImVec2( 130.f, 30.f ) ) )
                {
                    Editor::RuntimeManager::CompileRuntime( );
                }

                ImGui::SameLine( );

				if ( ImGui::Button( Editor::RuntimeManager::GetProjectRuntimeLibrary( ) == nullptr ? "Load binary" : "Unload binary", ImVec2( 130.f, 30.f ) ) )
				{
					if ( Editor::RuntimeManager::GetProjectRuntimeLibrary( ) == nullptr )
						Editor::RuntimeManager::LoadRuntimeLibrary( );
					else
						Editor::RuntimeManager::UnloadRuntimeLibrary( );
				}

				ImGui::Spacing( );
				ImGui::Separator( );
				ImGui::Spacing( );

				ImGui::PushFont( Gui::Fonts::BoldFont );
				ImGui::Text( "Physics" );
				ImGui::PopFont( );

				ImGui::Text( "Rigid body(s) in scene: %d", Pine::PhysicsManager->GetPhysicsWorld( )->getNbRigidBodies( ) );
				ImGui::Text( "Collision body(s) in scene: %d", Pine::PhysicsManager->GetPhysicsWorld( )->getNbCollisionBodies( ) );

				ImGui::EndTabItem( );
			}

			if ( ImGui::BeginTabItem( "Editor Settings" ) )
			{

				ImGui::EndTabItem( );
			}

            if ( ImGui::BeginTabItem( "Game Settings" ) )
            {
                const auto properties = Pine::GameManager::GetProperties( );

                char gameName[128];
                char startupLevel[128];

                strcpy_s(gameName, properties->Name.c_str());
                strcpy_s(startupLevel, properties->StartupLevel.c_str());

                ImGui::Text("Name");

                if ( ImGui::InputText( "##GameName", gameName, 128 ) )
                {
                    properties->Name = gameName;
                }

                ImGui::Text("Version");

                ImGui::InputInt("##Version", &properties->Version);

                ImGui::Text("Startup level");
                if ( ImGui::InputText( "##StartupLevel", startupLevel, 128 ) )
                {
                    properties->StartupLevel = startupLevel;
                }

                ImGui::SameLine();

                if (ImGui::Button("Set Current"))
                {
                    if (Editor::ProjectManager::GetCurrentLevel())
                        properties->StartupLevel = Editor::ProjectManager::GetCurrentLevel()->GetPath().string();
                }

                ImGui::Spacing();
                ImGui::Spacing();
                ImGui::Spacing();

                if (ImGui::Button("Save"))
                {
                    Pine::GameManager::Save(Editor::ProjectManager::GetCurrentProjectDirectory() + "\\game.asset");
                }

                ImGui::EndTabItem( );
            }


			ImGui::EndTabBar( );
		}


		ImGui::EndPopup( );
	}

}