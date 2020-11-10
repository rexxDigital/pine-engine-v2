#include "Widgets.hpp"
#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h>
#include "Pine/Assets/Texture2D/Texture2D.hpp"

void Editor::Gui::Widgets::Vector3( const std::string& str, glm::vec3& vec ) {

	const float size = 60.f;

	ImGui::Columns( 2, nullptr, false );

	ImGui::Text( "%s", str.c_str( ) );

	ImGui::NextColumn( );

	ImGui::BeginChild( str.c_str( ), ImVec2( -1.f, 30.f ), false, 0 );

	ImGui::Columns( 3, nullptr, false );

	ImGui::SetNextItemWidth( size );
	ImGui::DragFloat( std::string( "X##" + str ).c_str( ), &vec.x, 0.01f, -FLT_MAX, FLT_MAX );

	ImGui::NextColumn( );

	ImGui::SetNextItemWidth( size );
	ImGui::DragFloat( std::string( "Y##" + str ).c_str( ), &vec.y, 0.01f, -FLT_MAX, FLT_MAX );

	ImGui::NextColumn( );

	ImGui::SetNextItemWidth( size );
	ImGui::DragFloat( std::string( "Z##" + str ).c_str( ), &vec.z, 0.01f, -FLT_MAX, FLT_MAX );

	ImGui::Columns( 1 );

	ImGui::EndChild( );

	ImGui::Columns( 1 );

}

void Editor::Gui::Widgets::PushDisabled( ) {
	
	ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
	ImGui::PushStyleVar( ImGuiStyleVar_Alpha, ImGui::GetStyle( ).Alpha * 0.3f );

}

void Editor::Gui::Widgets::PopDisabled( ) {

	ImGui::PopItemFlag( );
	ImGui::PopStyleVar( );

}

Pine::IAsset* Editor::Gui::Widgets::AssetPicker( const std::string& str, Pine::IAsset* currentAsset /*= nullptr*/, bool shouldRestrictType /*= false*/, Pine::EAssetType type /*= Pine::EAssetType::Invalid */ ) {
	Pine::IAsset* returnValue = currentAsset;

	ImGui::Columns( 2, nullptr, false );

	ImGui::Text( "%s", str.c_str( ) );

	ImGui::NextColumn( );

	char buff[ 64 ];

	strcpy_s( buff, "\0" );

	
	ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail( ).x - 30.f );
	ImGui::InputText( std::string( "##" + str ).c_str( ), buff, 64, 0 );
	
	ImGui::SameLine( );

	if ( currentAsset == nullptr ) {
		Widgets::PushDisabled( );
	}

	if ( ImGui::Button( "X" ) ) {
		returnValue = nullptr;
	}

	if ( currentAsset == nullptr ) {
		Widgets::PopDisabled( );
	}

	ImGui::Columns( 1 );
	
	return returnValue;
}

bool Editor::Gui::Widgets::Checkbox( const std::string& str, bool& value ) {
	ImGui::Columns( 2, nullptr, false );

	ImGui::Text( "%s", str.c_str( ) );

	ImGui::NextColumn( );

	bool ret = ImGui::Checkbox( std::string( "##" + str ).c_str( ), &value );

	ImGui::Columns( 1 );

	return ret;
}

bool Editor::Gui::Widgets::Icon( const std::string& text, bool showBackground, Pine::Texture2D* texture, int size ) {	
	bool ret = false;

	ImGui::PushID( text.c_str( ) );
	ImGui::BeginGroup( );

	if ( !showBackground ) {
		ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
	}

	if ( ImGui::ImageButton( reinterpret_cast< ImTextureID >( texture->GetId( ) ), ImVec2( size, size ), ImVec2( 0.f, 0.f ), ImVec2( 1.f, 1.f ), 3 ) ) {
		ret = true;
	}

	if ( !showBackground )
		ImGui::PopStyleColor( );

	ImGui::Text( text.c_str( ) );

	ImGui::EndGroup( );
	ImGui::PopID( );

	return ret;
}
