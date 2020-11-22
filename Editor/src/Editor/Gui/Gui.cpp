#include "Gui.hpp"

#include <Pine\Gui\Gui.hpp>
#include <ImGui\imgui.h>

#include "Windows/Windows.hpp"
#include "MainMenuBar/MainMenuBar.hpp"
#include "..\ProjectManager\ProjectManager.hpp"

#include <ImGuizmo/ImGuizmo.h>

namespace {

	void SetupDockspace( ) {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		ImGuiViewport* viewport = ImGui::GetMainViewport( );

		ImGui::SetNextWindowPos( viewport->GetWorkPos( ) );
		ImGui::SetNextWindowSize( viewport->GetWorkSize( ) );
		ImGui::SetNextWindowViewport( viewport->ID );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );
		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );

		ImGui::Begin( "PineDockSpaceWindow", nullptr, window_flags );

		ImGui::PopStyleVar( 3 );

		ImGuiIO& io = ImGui::GetIO( );

		ImGuiID dockspace_id = ImGui::GetID( "DockSpace" );

		ImGui::DockSpace( dockspace_id, ImVec2( 0.0f, 0.0f ), ImGuiDockNodeFlags_None );

		ImGui::End( );
	}

	void OnRenderGui( ) {
		ImGuizmo::BeginFrame();
		
		if ( !Editor::ProjectManager::HasProjectOpen( ) ) {
			Editor::Gui::Windows::RenderProjectWizard( );

			return;
		}

		SetupDockspace( );

		ImGui::ShowDemoWindow( );
		Editor::Gui::MainMenuBar::Render( );
		Editor::Gui::Windows::RenderEntitylist( );
		Editor::Gui::Windows::RenderViewports( );
		Editor::Gui::Windows::RenderAssetBrowser( );
		Editor::Gui::Windows::RenderProperties( );
		Editor::Gui::Windows::RenderDebugWindows( );

	}

}

void Editor::Gui::Setup( ) {

	ImGuiIO& io = ImGui::GetIO( );

	Fonts::TitleFont = io.Fonts->AddFontFromFileTTF( "Assets\\Engine\\OpenSans-Regular.ttf", 34.f );

	Pine::Gui::SetGuiRenderCallback( OnRenderGui );

}