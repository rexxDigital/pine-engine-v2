#include "Windows.hpp"
#include <Pine\Rendering\RenderManager\RenderManager.hpp>
#include <Pine\Entity\Entity.hpp>

#include <Pine/Core/Log/Log.hpp>

using namespace Editor::Gui;

namespace
{

	void RenderMessage( const std::string& msg, Pine::Log::LogLevel level )
	{
		ImVec4 color;

		switch ( level )
		{
		case Pine::Log::LogLevel::Debug:
			color = ImVec4( 0.50f, 0.50f, 0.50f, 1.00f );
			break;
		case Pine::Log::LogLevel::Message:
			color = ImVec4( 1.f, 1.f, 1.f, 1.00f );
			break;
		case Pine::Log::LogLevel::Warning:
			color = ImVec4( 1.f, 0.5f, 0.f, 1.00f );
			break;
		case Pine::Log::LogLevel::Error:
			color = ImVec4( 1.f, 0.f, 0.f, 1.00f );
			break;
		case Pine::Log::LogLevel::Fatal:
			color = ImVec4( 1.f, 0.f, 0.f, 1.00f );
			break;
		}

		ImGui::TextColored( color, msg.c_str( ) );
	}

}

void Windows::RenderConsole( ) {

	if ( ImGui::Begin( "Console", &Windows::ShowConsole, 0 ) )
	{
		if ( ImGui::Button( "Clear" ) )
		{
			Pine::Log::ClearLogMessages( );
		}

		ImGui::PushStyleColor( ImGuiCol_::ImGuiCol_ChildBg, ImVec4( 0.04f, 0.04f, 0.04f, 1.00f ) );

		ImGui::BeginChild( "##ConsoleLogs", ImVec2( -1.f, -1.f ), false, ImGuiWindowFlags_::ImGuiWindowFlags_AlwaysAutoResize );

		const auto& msgs = Pine::Log::GetLogMessages( );

		for ( const auto& msg: msgs )
		{
			RenderMessage( msg.first, msg.second );
		}

		ImGui::EndChild( );

		ImGui::PopStyleColor( );

	}
	ImGui::End( );

}