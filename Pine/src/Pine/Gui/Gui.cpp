#include "Gui.hpp"
#include "../../ImGui/imgui.h"
#include "../../ImGui/imgui_impl_glfw.h"
#include "../../ImGui/imgui_impl_opengl3.h"
#include "../Core/Window/Window.hpp"

#include <GL/glew.h>
#include <GLFW\glfw3.h>

#include "../Core/Log/Log.hpp"
#include "../Components/Components.hpp"
#include "../Pine.hpp"

namespace Pine
{

	class CGui : public IGui
	{
	private:
		GuiRenderCallback g_GuiRenderCallback;

		ImGuiContext* g_GuiContext = nullptr;

		void SetupGuiStyle( ) {
			auto* style = &ImGui::GetStyle( );

			style->WindowPadding = ImVec2( 8.f, 8.f );
			style->FramePadding = ImVec2( 8.f, 4.f );
			style->ItemSpacing = ImVec2( 8.f, 4.f );
			style->ItemInnerSpacing = ImVec2( 4.f, 4.f );
			style->TouchExtraPadding = ImVec2( 0.f, 0.f );
			style->IndentSpacing = 25.f;
			style->ScrollbarSize = 10.f;
			style->GrabMinSize = 6.f;

			style->ChildBorderSize = 1.f;
			style->FrameBorderSize = 0.f;
			style->PopupBorderSize = 1.f;
			style->TabBorderSize = 0.f;
			style->WindowBorderSize = 0.f;

			style->ChildRounding = 0.f;
			style->FrameRounding = 0.f;
			style->GrabRounding = 0.f;
			style->PopupRounding = 0.f;
			style->ScrollbarRounding = 0.f;
			style->TabRounding = 0.f;
			style->WindowRounding = 0.f;

			ImVec4* colors = ImGui::GetStyle( ).Colors;

			colors[ ImGuiCol_Text ] = ImVec4( 0.93f, 0.93f, 0.93f, 1.00f );
			colors[ ImGuiCol_TextDisabled ] = ImVec4( 0.41f, 0.41f, 0.41f, 1.00f );
			colors[ ImGuiCol_WindowBg ] = ImVec4( 0.06f, 0.06f, 0.06f, 1.00f );
			colors[ ImGuiCol_ChildBg ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
			colors[ ImGuiCol_PopupBg ] = ImVec4( 0.05f, 0.05f, 0.05f, 1.00f );
			colors[ ImGuiCol_Border ] = ImVec4( 0.10f, 0.10f, 0.10f, 1.00f );
			colors[ ImGuiCol_BorderShadow ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
			colors[ ImGuiCol_FrameBg ] = ImVec4( 0.10f, 0.10f, 0.10f, 1.00f );
			colors[ ImGuiCol_FrameBgHovered ] = ImVec4( 0.11f, 0.11f, 0.11f, 1.00f );
			colors[ ImGuiCol_FrameBgActive ] = ImVec4( 0.21f, 0.21f, 0.21f, 1.00f );
			colors[ ImGuiCol_TitleBg ] = ImVec4( 0.06f, 0.06f, 0.06f, 1.00f );
			colors[ ImGuiCol_TitleBgActive ] = ImVec4( 0.06f, 0.06f, 0.06f, 1.00f );
			colors[ ImGuiCol_TitleBgCollapsed ] = ImVec4( 0.06f, 0.06f, 0.06f, 1.00f );
			colors[ ImGuiCol_MenuBarBg ] = ImVec4( 0.06f, 0.06f, 0.06f, 1.00f );
			colors[ ImGuiCol_ScrollbarBg ] = ImVec4( 0.02f, 0.02f, 0.02f, 0.53f );
			colors[ ImGuiCol_ScrollbarGrab ] = ImVec4( 0.13f, 0.13f, 0.13f, 1.00f );
			colors[ ImGuiCol_ScrollbarGrabHovered ] = ImVec4( 0.41f, 0.41f, 0.41f, 1.00f );
			colors[ ImGuiCol_ScrollbarGrabActive ] = ImVec4( 0.51f, 0.51f, 0.51f, 1.00f );
			colors[ ImGuiCol_CheckMark ] = ImVec4( 0.41f, 0.98f, 0.26f, 1.00f );
			colors[ ImGuiCol_SliderGrab ] = ImVec4( 0.32f, 0.88f, 0.24f, 1.00f );
			colors[ ImGuiCol_SliderGrabActive ] = ImVec4( 0.37f, 0.98f, 0.26f, 1.00f );
			colors[ ImGuiCol_Button ] = ImVec4( 0.26f, 0.98f, 0.35f, 0.40f );
			colors[ ImGuiCol_ButtonHovered ] = ImVec4( 0.15f, 0.56f, 0.17f, 1.00f );
			colors[ ImGuiCol_ButtonActive ] = ImVec4( 0.08f, 0.54f, 0.00f, 1.00f );
			colors[ ImGuiCol_Header ] = ImVec4( 0.37f, 0.98f, 0.26f, 0.31f );
			colors[ ImGuiCol_HeaderHovered ] = ImVec4( 0.17f, 0.72f, 0.20f, 0.80f );
			colors[ ImGuiCol_HeaderActive ] = ImVec4( 0.24f, 0.56f, 0.15f, 1.00f );
			colors[ ImGuiCol_Separator ] = ImVec4( 0.15f, 0.15f, 0.15f, 0.50f );
			colors[ ImGuiCol_SeparatorHovered ] = ImVec4( 0.16f, 0.16f, 0.16f, 0.50f );
			colors[ ImGuiCol_SeparatorActive ] = ImVec4( 0.16f, 0.16f, 0.16f, 0.50f );
			colors[ ImGuiCol_ResizeGrip ] = ImVec4( 0.28f, 0.98f, 0.26f, 0.25f );
			colors[ ImGuiCol_ResizeGripHovered ] = ImVec4( 0.39f, 0.98f, 0.26f, 0.67f );
			colors[ ImGuiCol_ResizeGripActive ] = ImVec4( 0.50f, 0.98f, 0.26f, 0.95f );
			colors[ ImGuiCol_Tab ] = ImVec4( 0.08f, 0.17f, 0.08f, 1.00f );
			colors[ ImGuiCol_TabHovered ] = ImVec4( 0.22f, 0.51f, 0.21f, 1.00f );
			colors[ ImGuiCol_TabActive ] = ImVec4( 0.19f, 0.43f, 0.17f, 1.00f );
			colors[ ImGuiCol_TabUnfocused ] = ImVec4( 0.08f, 0.17f, 0.08f, 1.00f );
			colors[ ImGuiCol_TabUnfocusedActive ] = ImVec4( 0.19f, 0.43f, 0.17f, 1.00f );
			colors[ ImGuiCol_DockingPreview ] = ImVec4( 0.32f, 0.98f, 0.26f, 0.70f );
			colors[ ImGuiCol_DockingEmptyBg ] = ImVec4( 0.20f, 0.20f, 0.20f, 1.00f );
			colors[ ImGuiCol_PlotLines ] = ImVec4( 0.61f, 0.61f, 0.61f, 1.00f );
			colors[ ImGuiCol_PlotLinesHovered ] = ImVec4( 1.00f, 0.43f, 0.35f, 1.00f );
			colors[ ImGuiCol_PlotHistogram ] = ImVec4( 0.90f, 0.70f, 0.00f, 1.00f );
			colors[ ImGuiCol_PlotHistogramHovered ] = ImVec4( 1.00f, 0.60f, 0.00f, 1.00f );
			colors[ ImGuiCol_TableHeaderBg ] = ImVec4( 0.19f, 0.19f, 0.20f, 1.00f );
			colors[ ImGuiCol_TableBorderStrong ] = ImVec4( 0.31f, 0.31f, 0.35f, 1.00f );
			colors[ ImGuiCol_TableBorderLight ] = ImVec4( 0.23f, 0.23f, 0.25f, 1.00f );
			colors[ ImGuiCol_TableRowBg ] = ImVec4( 0.00f, 0.00f, 0.00f, 0.00f );
			colors[ ImGuiCol_TableRowBgAlt ] = ImVec4( 1.00f, 1.00f, 1.00f, 0.06f );
			colors[ ImGuiCol_TextSelectedBg ] = ImVec4( 0.26f, 0.59f, 0.98f, 0.35f );
			colors[ ImGuiCol_DragDropTarget ] = ImVec4( 1.00f, 1.00f, 0.00f, 0.90f );
			colors[ ImGuiCol_NavHighlight ] = ImVec4( 0.26f, 0.59f, 0.98f, 1.00f );
			colors[ ImGuiCol_NavWindowingHighlight ] = ImVec4( 1.00f, 1.00f, 1.00f, 0.70f );
			colors[ ImGuiCol_NavWindowingDimBg ] = ImVec4( 0.80f, 0.80f, 0.80f, 0.20f );
			colors[ ImGuiCol_ModalWindowDimBg ] = ImVec4( 0.80f, 0.80f, 0.80f, 0.35f );
		}

	public:

		void SetGuiRenderCallback( GuiRenderCallback callback ) override
		{
			g_GuiRenderCallback = callback;
		}

		void Setup( ) override
		{
			Log->Debug( "Pine::Gui->Setup()" );

			g_GuiContext = ImGui::CreateContext( );

			ImGui_ImplGlfw_InitForOpenGL( Window::Internal::GetWindowPointer( ), true );
			ImGui_ImplOpenGL3_Init( "#version 420" );

			ImGuiIO& io = ImGui::GetIO( );

			SetupGuiStyle( );

			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

			io.Fonts->AddFontFromFileTTF( "Assets\\Engine\\OpenSans-Regular.ttf", 17.f );
		}

		void Dispose( ) override
		{
			ImGui_ImplOpenGL3_Shutdown( );
			ImGui_ImplGlfw_Shutdown( );
			ImGui::DestroyContext( );
		}

		void Render( ) override
		{
			// Reset the frame buffer for rendering.
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );

			ImGui::SetCurrentContext( g_GuiContext );

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame( );
			ImGui_ImplGlfw_NewFrame( );
			ImGui::NewFrame( );

			if ( g_GuiRenderCallback ) 
			{
				g_GuiRenderCallback( );
			}
			
			glViewport( 0, 0, Window::GetCachedSize( ).x, Window::GetCachedSize( ).y );

			ImGui::Render( );
			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData( ) );
		}

	};

	IGui* CreateGuiInterface( )
	{
		return new CGui( );
	}

}

