#include "Windows.hpp"
#include "../../ProjectManager/ProjectManager.hpp"
#include "../Gui.hpp"
#include <Pine/Core/Window/Window.hpp>

namespace {

	void ShowCreateProjectPopup( ) {
		static char projectNameBuff[ 64 ];

		if ( ImGui::BeginPopupModal( "CreateNewProject", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize ) ) {
			ImGui::Text( "Project name:" );
			ImGui::InputText( "##ProjectName", projectNameBuff, 64 );

			if ( ImGui::Button( "OK" ) || ImGui::IsKeyPressed( ImGui::GetKeyIndex( ImGuiKey_::ImGuiKey_Enter ) ) ) {
				if ( strlen( projectNameBuff ) > 1 ) {
					Editor::ProjectManager::CreateProject( "Projects\\" + std::string( projectNameBuff ) );
					ImGui::CloseCurrentPopup( );
				}
			}

			ImGui::SameLine( );

			if ( ImGui::Button( "Cancel" ) ) {
				ImGui::CloseCurrentPopup( );
			}

			ImGui::EndPopup( );
		}
	}
}

void Editor::Gui::Windows::RenderProjectWizard( ) {

	ImGui::SetNextWindowPos( ImVec2( 0.f, 0.f ) );
	ImGui::SetNextWindowSize( ImVec2( 700.f, 400.f ) );
	ImGui::Begin( "Project Wizard", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar );

	ImGui::PushFont( Fonts::TitleFont );
	ImGui::Text( "Project Wizard" );
	ImGui::PopFont( );

	ImGui::Columns( 2, nullptr, false );

	ImGui::Text( "Available projects:" );

	for ( auto& project : ProjectManager::GetAvailableProjects( ) ) {
		if ( ImGui::Button( project.c_str( ), ImVec2( -1.f, 30.f ) ) ) {
			ProjectManager::OpenProject( project );
		}
	}

	ImGui::NextColumn( );

	if ( ImGui::Button( "Create new project...", ImVec2( -1.f, 40.f ) ) ) {
		ImGui::OpenPopup( "CreateNewProject" );
	}

	if ( ImGui::Button( "Open existing project...", ImVec2( -1.f, 40.f ) ) ) {
		// TODO: Fix me :^)
	}

	if ( ImGui::Button( "Exit", ImVec2( -1.f, 40.f ) ) ) {
		Pine::Window::Close( );
	}

	ShowCreateProjectPopup( );

	ImGui::End( );


}