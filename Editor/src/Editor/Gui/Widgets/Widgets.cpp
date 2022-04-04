#include "Widgets.hpp"
#include <imgui/imgui.h>
#include <ImGui/imgui_internal.h>
#include "Pine/Assets/Texture2D/Texture2D.hpp"
#include <optional>
#include <windows.h>
#include <Pine/Core/Window/Window.hpp>
#include <Pine/Assets/Assets.hpp>
#include <Pine/Assets/IAsset/IAsset.hpp>
#include <filesystem>
#include "../Gui.hpp"
#include "Pine/Entity/Entity.hpp"


namespace
{
	std::optional<std::string> OpenFile( const char* filter )
	{
		OPENFILENAMEA ofn;
		CHAR szFile[ 260 ] = { 0 };
		ZeroMemory( &ofn, sizeof( OPENFILENAME ) );
		ofn.lStructSize = sizeof( OPENFILENAME );
		ofn.hwndOwner = reinterpret_cast< HWND >( Pine::Window::Internal::GetWindowHWND( ) );
		ofn.lpstrFile = szFile;
		ofn.nMaxFile = sizeof( szFile );
		ofn.lpstrFilter = filter;
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

		if ( GetOpenFileNameA( &ofn ) == TRUE )
			return ofn.lpstrFile;

		return std::nullopt;
	}

	Pine::IAsset* FindAssetFromAbsolutePath( const std::string& path )
	{
		for ( auto& a : Pine::Assets->GetAssets( ) )
		{
			if ( a.second->GetAbsoluteFilePath( ) == path )
			{
				return a.second;
			}
		}

		return nullptr;
	}
}

bool Editor::Gui::Widgets::Vector3( const std::string& str, glm::vec3& vec )
{
	const float size = 50.f;

	ImGui::Columns( 2, nullptr, false );

	ImGui::Text( "%s", str.c_str( ) );

	ImGui::NextColumn( );

	ImGui::BeginChild( str.c_str( ), ImVec2( -1.f, 30.f ), false, 0 );

	ImGui::Columns( 3, nullptr, false );

	ImGui::SetNextItemWidth( size );
	const bool xChanged = ImGui::DragFloat( std::string( "X##" + str ).c_str( ), &vec.x, 0.1f, -FLT_MAX, FLT_MAX );

	ImGui::NextColumn( );

	ImGui::SetNextItemWidth( size );
	const bool yChanged = ImGui::DragFloat( std::string( "Y##" + str ).c_str( ), &vec.y, 0.1f, -FLT_MAX, FLT_MAX );

	ImGui::NextColumn( );

	ImGui::SetNextItemWidth( size );
	const bool zChanged = ImGui::DragFloat( std::string( "Z##" + str ).c_str( ), &vec.z, 0.1f, -FLT_MAX, FLT_MAX );

	ImGui::Columns( 1 );

	ImGui::EndChild( );

	ImGui::Columns( 1 );

	return xChanged || yChanged || zChanged;
}

bool Editor::Gui::Widgets::SliderFloat( const std::string& str, float& value, float min, float max, const std::string& format )
{
	ImGui::Columns( 2, nullptr, false );

	ImGui::Text( "%s", str.c_str( ) );

	ImGui::NextColumn( );

	ImGui::SetNextItemWidth( -1.f );

	const bool retValue = ImGui::SliderFloat( std::string( "##" + str ).c_str( ), &value, min, max, format.c_str( ) );

	ImGui::Columns( 1 );

	return retValue;
}

bool Editor::Gui::Widgets::Combobox( const std::string& str, int& value, const char* items )
{
	ImGui::Columns( 2, nullptr, false );

	ImGui::Text( "%s", str.c_str( ) );

	ImGui::NextColumn( );

	ImGui::SetNextItemWidth( -1.f );

	const bool retValue = ImGui::Combo( std::string( "##" + str ).c_str( ), &value, items );

	ImGui::Columns( 1 );

	return retValue;
}

void Editor::Gui::Widgets::PushDisabled( )
{

	ImGui::PushItemFlag( ImGuiItemFlags_Disabled, true );
	ImGui::PushStyleVar( ImGuiStyleVar_Alpha, ImGui::GetStyle( ).Alpha * 0.3f );

}

void Editor::Gui::Widgets::PopDisabled( )
{

	ImGui::PopItemFlag( );
	ImGui::PopStyleVar( );

}

PickerReturn Editor::Gui::Widgets::AssetPicker( const std::string& str, Pine::IAsset* currentAsset /*= nullptr*/, bool shouldRestrictType /*= false*/, Pine::AssetType type /*= Pine::AssetType::Invalid */ )
{
	PickerReturn returnValue;

	ImGui::Columns( 2, nullptr, false );

	ImGui::Text( "%s", str.c_str( ) );

	ImGui::NextColumn( );

	char buff[ 255 ];

	if ( currentAsset != nullptr )
	{
		strcpy_s( buff, currentAsset->GetPath( ).string( ).c_str( ) );
	}
	else
	{
		strcpy_s( buff, "\0" );
	}

	ImGui::PushStyleVar( ImGuiStyleVar_::ImGuiStyleVar_ItemSpacing, ImVec2( 3.f, 4.f ) );

	ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail( ).x - 60.f );
	ImGui::InputText( std::string( "##" + str ).c_str( ), buff, 64, ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly );

	if ( ImGui::BeginDragDropTarget( ) )
	{
		if ( const auto payload = ImGui::AcceptDragDropPayload( "Asset", 0 ) )
		{
			const auto asset = *reinterpret_cast< Pine::IAsset** >( payload->Data );

			if ( !shouldRestrictType || ( shouldRestrictType && asset->GetType( ) == type ) )
			{
				returnValue.asset = asset;
				returnValue.valid = true;
			}
		}

		ImGui::EndDragDropTarget( );
	}

	if ( ImGui::IsItemHovered( ) && currentAsset != nullptr && currentAsset->GetType( ) == Pine::AssetType::Texture2D )
	{
		const auto texture = dynamic_cast< Pine::Texture2D* >( currentAsset );

		ImGui::BeginTooltip( );

		ImGui::Text( currentAsset->GetPath( ).string( ).c_str( ) );

		ImGui::Image( reinterpret_cast< ImTextureID >( texture->GetId( ) ), ImVec2( 64.f, 64.f ) );

		ImGui::EndTooltip( );
	}
	else if ( ImGui::IsItemHovered( ) && currentAsset != nullptr )
	{
		ImGui::BeginTooltip( );

		ImGui::Text( currentAsset->GetPath( ).string( ).c_str( ) );

		ImGui::EndTooltip( );
	}

	if ( ImGui::IsItemClicked( ) && currentAsset != nullptr )
	{
		Editor::Gui::Globals::SelectedAssetPtrs.clear( );
		Editor::Gui::Globals::SelectedEntityPtrs.clear( );
		Editor::Gui::Globals::SelectedAssetPtrs.push_back( currentAsset );
	}

	ImGui::SameLine( );

	if ( ImGui::Button( std::string( "...##" + str ).c_str( ) ) )
	{
		auto file = OpenFile( "Asset File (*.*)\0*.*\0" );
		if ( file )
		{
			if ( auto a = FindAssetFromAbsolutePath( *file ) )
			{
				if ( shouldRestrictType && a->GetType( ) == type )
				{
					returnValue.asset = a;
					returnValue.valid = true;
				}
			}
			else
			{
				MessageBoxA( reinterpret_cast< HWND >( Pine::Window::Internal::GetWindowHWND( ) ), "Asset is not loaded.", 0, 0 );
			}
		}
	}

	if ( ImGui::IsItemHovered( ) )
		ImGui::SetTooltip( "Browse Asset" );

	ImGui::SameLine( );

	if ( currentAsset == nullptr )
		PushDisabled( );

	if ( ImGui::Button( std::string( "X##" + str ).c_str( ) ) )
	{
		// This will cause it to update the asset as a nullptr for us later.

		returnValue.valid = true;
	}

	if ( ImGui::IsItemHovered( ) )
		ImGui::SetTooltip( "Remove Asset" );

	if ( currentAsset == nullptr )
		PopDisabled( );

	ImGui::PopStyleVar( );

	ImGui::Columns( 1 );

	return returnValue;
}

PickerReturn Editor::Gui::Widgets::EntityPicker( const std::string& str, Pine::Entity* currentEntity )
{
	PickerReturn returnValue;

	ImGui::Columns( 2, nullptr, false );

	ImGui::Text( "%s", str.c_str( ) );

	ImGui::NextColumn( );

	char buff[ 64 ];

	if ( currentEntity != nullptr )
	{
		strcpy_s( buff, currentEntity->GetName( ).c_str( ) );
	}
	else
	{
		strcpy_s( buff, "\0" );
	}

	ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 3.f, 4.f ) );

	ImGui::SetNextItemWidth( ImGui::GetContentRegionAvail( ).x - 31 );
	ImGui::InputText( std::string( "##" + str ).c_str( ), buff, 64, ImGuiInputTextFlags_ReadOnly );

	if ( ImGui::BeginDragDropTarget( ) )
	{
		if ( const auto payload = ImGui::AcceptDragDropPayload( "Entity", 0 ) )
		{
			const auto entity = *reinterpret_cast< Pine::Entity** >( payload->Data );

			returnValue.valid = true;
			returnValue.entity = entity;
		}

		ImGui::EndDragDropTarget( );
	}

	ImGui::SameLine( );

	if ( ImGui::Button( std::string( "X##" + str ).c_str( ) ) )
	{
		returnValue.valid = true;
	}

	ImGui::PopStyleVar( );

	ImGui::Columns( 1 );

	return returnValue;
}

bool Editor::Gui::Widgets::Checkbox( const std::string& str, bool& value )
{
	ImGui::Columns( 2, nullptr, false );

	ImGui::Text( "%s", str.c_str( ) );

	ImGui::NextColumn( );

	bool ret = ImGui::Checkbox( std::string( "##" + str ).c_str( ), &value );

	ImGui::Columns( 1 );

	return ret;
}

bool Editor::Gui::Widgets::Icon( const std::string& text, bool showBackground, Pine::Texture2D* texture, int size, Pine::IAsset* asset, PathItem_t* directory, bool flipImage )
{
	bool ret = false;

	ImGui::PushID( text.c_str( ) );
	ImGui::BeginGroup( );

	if ( !showBackground )
	{
		ImGui::PushStyleColor( ImGuiCol_Button, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
	}

	if ( ImGui::ImageButton( reinterpret_cast< ImTextureID >( texture->GetId( ) ), ImVec2( size, size ), ImVec2( 0.f, flipImage ? 1.f : 0.f ), ImVec2( 1.f, flipImage ? 0.f : 1.f ), 3 ) )
	{
		ret = true;
	}

	if ( asset != nullptr )
	{
		if ( ImGui::BeginDragDropSource( ImGuiDragDropFlags_::ImGuiDragDropFlags_None ) )
		{
			ImGui::SetDragDropPayload( "Asset", &asset, sizeof( Pine::IAsset* ), 0 );

			ImGui::Image( reinterpret_cast< ImTextureID >( texture->GetId( ) ), ImVec2( 64.f, 64.f ) );

			ImGui::SameLine( );

			ImGui::BeginChild( "AssetInfo", ImVec2( Fonts::BoldFont->CalcTextSizeA( 24.f, FLT_MAX, -1.f, text.c_str( ), nullptr, nullptr ).x + 2.f, -1.f ) );

			ImGui::PushFont( Fonts::BoldFont );

			ImGui::Text( text.c_str( ) );

			ImGui::PopFont( );

			ImGui::TextColored( ImVec4( 0.6f, 0.6f, 0.6f, 1.f ), "%s", Pine::SAssetType[ static_cast< int >( asset->GetType( ) ) ] );

			ImGui::EndChild( );

			ImGui::EndDragDropSource( );
		}
	}
    else
    {
        if ( ImGui::BeginDragDropSource( ImGuiDragDropFlags_::ImGuiDragDropFlags_None ) )
        {
            ImGui::SetDragDropPayload( "Directory", &directory, sizeof( PathItem_t* ), 0 );

            ImGui::Image( reinterpret_cast< ImTextureID >( texture->GetId( ) ), ImVec2( 64.f, 64.f ) );

            ImGui::SameLine( );

            ImGui::BeginChild( "DirectoryInfo", ImVec2( Fonts::BoldFont->CalcTextSizeA( 24.f, FLT_MAX, -1.f, text.c_str( ), nullptr, nullptr ).x + 2.f, -1.f ) );

            ImGui::PushFont( Fonts::BoldFont );

            ImGui::Text( text.c_str( ) );

            ImGui::PopFont( );

            ImGui::EndChild( );

            ImGui::EndDragDropSource( );
        }
    }

	if ( !showBackground )
		ImGui::PopStyleColor( );

	ImGui::SetCursorPosX( ImGui::GetCursorPosX( ) + ( size / 2.f - ( min( ImGui::CalcTextSize( text.c_str( ) ).x, 64.f ) / 2.f ) ) + 2.f );
	ImGui::TextWrapped( text.c_str( ) );

	ImGui::EndGroup( );
	ImGui::PopID( );

	return ret;
}

bool Editor::Gui::Widgets::ColorPicker( const std::string& str, glm::vec3& vec )
{
	ImGui::Columns( 2, nullptr, false );

	ImGui::Text( "%s", str.c_str( ) );

	ImGui::NextColumn( );

	ImGui::SetNextItemWidth( -1.f );

	const bool ret = ImGui::ColorEdit3( std::string( "##" + str ).c_str( ), &vec.x );

	ImGui::Columns( 1 );

	return ret;
}
