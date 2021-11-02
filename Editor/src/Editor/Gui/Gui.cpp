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
		const ImGuiViewport* viewport = ImGui::GetMainViewport( );

		ImGui::SetNextWindowPos( viewport->WorkPos );
		ImGui::SetNextWindowSize( ImVec2( viewport->WorkSize.x, viewport->WorkSize.y ) );
		ImGui::SetNextWindowViewport( viewport->ID );

		ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );

		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );

		ImGui::Begin( "PineDockSpaceWindow", nullptr, window_flags );

		ImGui::PopStyleVar( 3 );

		const ImGuiID dockspaceId = ImGui::GetID( "DockSpace" );

		ImGui::DockSpace( dockspaceId, ImVec2( 0.0f, 0.f ), ImGuiDockNodeFlags_None );

		ImGui::End( );
	}

	void OnRenderGui( ) {
		ImGuizmo::BeginFrame( );

		if ( !Editor::ProjectManager::HasProjectOpen( ) ) {
			Editor::Gui::Windows::RenderProjectWizard( );

			return;
		}

		SetupDockspace( );

		ImGui::ShowDemoWindow( );
		Editor::Gui::MainMenuBar::Render( );

		// Because ImGui selects the last window within a docking space as the selected space,
		// I'll just call them in a order that makes the window I want to be shown by default to be shown first.
		// There might be some smart workaround for this, but I'll just do this for now.

		Editor::Gui::Windows::RenderEntitylist( );
		Editor::Gui::Windows::RenderViewports( );

		Editor::Gui::Windows::RenderConsole( );
		Editor::Gui::Windows::RenderAssetBrowser( ); // show 'asset browser' as the selected window.

		Editor::Gui::Windows::RenderLevelSettings( );
		Editor::Gui::Windows::RenderProperties( ); // show 'properties' as the selected window.
		
		Editor::Gui::Windows::RenderDebugWindows( );
	}

}

void Editor::Gui::Setup( ) {

	const ImGuiIO& io = ImGui::GetIO( );

	Fonts::TitleFont = io.Fonts->AddFontFromFileTTF( "Assets\\Engine\\OpenSans-Regular.ttf", 34.f );
	Fonts::BoldFont = io.Fonts->AddFontFromFileTTF( "Assets\\Engine\\OpenSans-Regular.ttf", 24.f );

	Fonts::CodeFont = io.Fonts->AddFontFromFileTTF( "Assets\\Engine\\Consolas.ttf", 13.f );

	Pine::Gui::SetGuiRenderCallback( OnRenderGui );

}