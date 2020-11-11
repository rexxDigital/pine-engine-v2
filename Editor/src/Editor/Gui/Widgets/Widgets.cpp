#include "Widgets.hpp"
#include <imgui/imgui.h>

void Editor::Gui::Widgets::Vector3( const std::string& str, glm::vec3& vec )
{
	const float size = 110.f;

	ImGui::Text( "%s", str.c_str( ) );

	ImGui::BeginChild( str.c_str( ), ImVec2( -1.f, -1.f ), false, 0 );

	ImGui::SetNextItemWidth( size );
	ImGui::DragFloat( std::string( "X##" + str ).c_str( ), &vec.x, 0.01f, -FLT_MAX, FLT_MAX );

	ImGui::SameLine( );

	ImGui::SetNextItemWidth( size );
	ImGui::DragFloat( std::string( "Y##" + str ).c_str( ), &vec.y, 0.01f, -FLT_MAX, FLT_MAX );

	ImGui::SameLine( );

	ImGui::SetNextItemWidth( size );
	ImGui::DragFloat( std::string( "Z##" + str ).c_str( ), &vec.z, 0.01f, -FLT_MAX, FLT_MAX );

	ImGui::EndChild( );
}
