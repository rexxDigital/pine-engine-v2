#include "Gui.hpp"
#include "ImGui/imgui.h"
#include "Pine/OpenGL/FrameBuffer/FrameBuffer.hpp"
#include "Windows/EntityList/Entitylist.hpp"
#include "Windows/Viewport/Viewport.hpp"
#include "Windows/Properties/Properties.hpp"
#include "Windows/AssetBrowser/AssetBrowser.hpp"
#include "Windows/Console/Console.hpp"
#include "Windows/Profiler/Profiler.hpp"

namespace {

	Pine::Entity* g_SelectedEntity = nullptr;
	Pine::IAsset* g_SelectedAsset = nullptr;

	Pine::FrameBuffer* g_ViewportFrameBuffer = nullptr;

	void SetupDockSpace( ) {
		ImGuiViewport* viewport = ImGui::GetMainViewport( );

		ImGui::SetNextWindowPos( viewport->GetWorkPos( ) );
		ImGui::SetNextWindowSize( viewport->GetWorkSize( ) );
		ImGui::SetNextWindowViewport( viewport->ID );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowRounding, 0.0f );
		ImGui::PushStyleVar( ImGuiStyleVar_WindowBorderSize, 0.0f );

		ImGui::Begin( "PineEditor", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking );

		ImGui::PopStyleVar( 2 );

		const ImGuiID dockspace_id = ImGui::GetID( "EngineDockSpace" );

		ImGui::DockSpace( dockspace_id, ImVec2( 0.0f, 0.0f ), ImGuiDockNodeFlags_None );
	}

	void EndDockingSpace( ) {
		ImGui::End( );
	}

}

Pine::FrameBuffer* Editor::Gui::GetViewportFrameBuffer( )
{
	return g_ViewportFrameBuffer;
}

void Editor::Gui::Setup( )
{
	g_ViewportFrameBuffer = new Pine::FrameBuffer( );
	g_ViewportFrameBuffer->Create( 700, 400 );
}

void Editor::Gui::Dispose( )
{
	g_ViewportFrameBuffer->Dispose( );
	delete g_ViewportFrameBuffer;
}

void Editor::Gui::Run( ) {

	SetupDockSpace( );

	//ImGui::ShowDemoWindow(nullptr);

	if ( ImGui::BeginMenuBar( ) ) {
		if ( ImGui::BeginMenu( "Options" ) ) {
			ImGui::EndMenu( );
		}

		ImGui::EndMenuBar( );
	}

	EntityList::Run( );
	Viewport::Run( );
	Properties::Run( );
	AssetBrowser::Run( );
	Console::Run( );
	Profiler::Run( );

	EndDockingSpace( );
}

Pine::Entity* Editor::Gui::GetSelectedEntity( ) {
	return g_SelectedEntity;
}

void Editor::Gui::SetSelectedEntity( Pine::Entity* entity ) {
	g_SelectedEntity = entity;
	g_SelectedAsset = nullptr;
}

Pine::IAsset* Editor::Gui::GetSelectedAsset( )
{
	return g_SelectedAsset;
}

void Editor::Gui::SetSelectedAsset( Pine::IAsset* asset )
{
	g_SelectedAsset = asset;
	g_SelectedEntity = nullptr;
}
