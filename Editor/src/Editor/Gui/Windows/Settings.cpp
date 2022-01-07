#include "Windows.hpp"
#include <Pine\Rendering\RenderManager\RenderManager.hpp>
#include <Pine\Entity\Entity.hpp>

#include "../Widgets/Widgets.hpp"
#include "../Gui.hpp"
#include "Editor/ProjectManager/ProjectManager.hpp"
#include "Pine/PhysicsManager/PhysicsManager.hpp"

#include "Pine/RuntimeLoader/RuntimeLoader.hpp"

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
				static bool autoReload = false;

				ImGui::PushFont( Gui::Fonts::BoldFont );
				ImGui::Text( "Runtime Library" );
				ImGui::PopFont( );

				ImGui::Text( "Runtime library target file: %s%s", Editor::ProjectManager::GetCurrentProjectDirectory( ).c_str( ), "\\GameRuntime.dll" );
				ImGui::Text( "Runtime library loaded: %s", Editor::ProjectManager::GetProjectRuntimeLibrary( ) != nullptr ? "Yes" : "No" );

				ImGui::Checkbox( "Auto Reload", &autoReload );

				if ( ImGui::Button( Editor::ProjectManager::GetProjectRuntimeLibrary( ) == nullptr ? "Load" : "Unload", ImVec2( 130.f, 30.f ) ) )
				{
					if ( Editor::ProjectManager::GetProjectRuntimeLibrary( ) == nullptr )
						Editor::ProjectManager::LoadRuntimeLibrary( );
					else
						Editor::ProjectManager::UnloadRuntimeLibrary( );
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


			ImGui::EndTabBar( );
		}


		ImGui::EndPopup( );
	}

}