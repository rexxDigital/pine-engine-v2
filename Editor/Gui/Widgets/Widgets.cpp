#include "Widgets.hpp"


#include <iostream>
#include <ImGui/imgui.h>
#include <Pine/Assets/Assets.hpp>



#include "../Gui.hpp"
#include "../../Utils/AssetPreviewGenerator/AssetPreviewGenerator.hpp"
#include "../../Utils/DirectoryCache/DirectoryCache.hpp"

void Gui::Widgets::Vector3( const std::string& str, glm::vec3& vec ) {
	ImGui::Text( str.c_str( ) );

	ImGui::Columns( 3, nullptr, false );

	ImGui::SetNextItemWidth( 80.f );
	ImGui::DragFloat( std::string( "X##" + str ).c_str( ), &vec.x, 0.1f, -FLT_MAX, FLT_MAX, "%.1f" );
	ImGui::NextColumn( );

	ImGui::SetNextItemWidth( 80.f );
	ImGui::DragFloat( std::string( "Y##" + str ).c_str( ), &vec.y, 0.1f, -FLT_MAX, FLT_MAX, "%.1f" );
	ImGui::NextColumn( );

	ImGui::SetNextItemWidth( 80.f );
	ImGui::DragFloat( std::string( "Z##" + str ).c_str( ), &vec.z, 0.1f, -FLT_MAX, FLT_MAX, "%.1f" );

	ImGui::Columns( 1 );
}

Pine::IAsset* Gui::Widgets::AssetPicker( Pine::IAsset* currentAsset, const std::string& str, Pine::EAssetType type ) {
	static auto unknownImage = Pine::Assets::GetAsset<Pine::Texture2D>( "Assets\\Engine\\Icons\\030-corrupt file.png" );
	std::string path = "<DROP ASSET HERE>";

	Pine::IAsset* returnValue = nullptr;
	
	if ( currentAsset )
	{
		path = currentAsset->GetPath( ).string( );
	}
	
	ImGui::Text( "%s:", str.c_str( ) );

	ImGui::BeginChild( str.c_str( ), ImVec2( -1.f, 64.f ), true );

	unsigned int image = unknownImage->GetId( );

	if ( currentAsset != nullptr )
	{
		if ( auto preview = Editor::AssetPreviewGenerator::GetAssetPreview( currentAsset ) )
		{
			image = preview->texture;
		}
	}

	ImGui::Image( reinterpret_cast< ImTextureID >( image ), ImVec2( 48, 48 ), ImVec2( 0.f, 0.f ), ImVec2( 1.f, 1.f ) );

	ImGui::SameLine( );

	ImGui::BeginChild( std::string( "ChildInner" + str ).c_str( ), ImVec2( -1.f, -1.f ), false );

	if ( currentAsset == nullptr )
	{
		ImGui::Text( "No asset selected." );
	}
	else
	{
		ImGui::Text( currentAsset->GetFileName( ).c_str( ) );
	}

	ImGui::InputText( std::string( "##AssetPicker" + str ).c_str( ), const_cast< char* >( path.data( ) ), path.size( ), ImGuiInputTextFlags_ReadOnly );

	if ( ImGui::BeginDragDropTarget( ) ) {
		if ( const ImGuiPayload* payload = ImGui::AcceptDragDropPayload( "_ASSET" ) ) {
			std::uintptr_t assetPtr = 0;
			memcpy( &assetPtr, payload->Data, sizeof( std::uintptr_t ) );
			auto asset = reinterpret_cast< Pine::IAsset* >( assetPtr );

			if ( asset->GetType(  ) == type )
			{
				returnValue = asset;
			}
		}

		ImGui::EndDragDropTarget( );
	}

	if ( currentAsset != nullptr )
	{
		ImGui::SameLine( );
		
		if ( ImGui::Button( "Open" ) ) {
			Editor::Gui::SetSelectedAsset( currentAsset );
		}
	}

	ImGui::EndChild( );
	ImGui::EndChild( );

	return returnValue;
}

namespace
{
	Editor::DirectoryCache::ItemCache_t* currentPath = nullptr;
	Editor::DirectoryCache::ItemCache_t* root = nullptr;

	void DisplayItem( Editor::DirectoryCache::ItemCache_t* item )
	{
		// Cached icons
		static auto folderIcon = Pine::Assets::GetAsset<Pine::Texture2D>( "Assets\\Engine\\Icons\\006-folder.png" );
		static auto unknownFileIcon = Pine::Assets::GetAsset<Pine::Texture2D>( "Assets\\Engine\\Icons\\030-corrupt file.png" );

		auto asset = item->assetPointer;

		const bool selected = Editor::Gui::GetSelectedAsset( ) != nullptr && Editor::Gui::GetSelectedAsset(  ) == asset;

		bool invertedUvs = false;
		int icon = unknownFileIcon->GetId( );
		std::string name = item->name;

		if ( item->isDirectory ) {
			icon = folderIcon->GetId( );
		}
		else {
			if ( asset )
			{
				if ( const auto preview = Editor::AssetPreviewGenerator::GetAssetPreview( asset ) )
				{
					icon = preview->texture;
					invertedUvs = preview->invertUVs;
				}
			}
		}

		ImGui::PushID( item->name.c_str( ) );
		ImGui::BeginGroup( );

		if ( !selected )
		{
			ImGui::PushStyleColor( ImGuiCol_::ImGuiCol_Button, ImVec4( 0.f, 0.f, 0.f, 0.f ) );
		}

		if ( ImGui::ImageButton( reinterpret_cast< ImTextureID >( icon ), ImVec2( 48, 48 ), invertedUvs ? ImVec2( 1.f, 1.f ) : ImVec2( 0.f, 0.f ), invertedUvs ? ImVec2( 0.f, 0.f ) : ImVec2( 1.f, 1.f ), 3 ) ) {
			if ( item->isDirectory ) {
				if ( item->name == "..." ) {
					currentPath = item->parent;
				}
				else {
					currentPath = item;
				}

				if ( !selected )
				{
					ImGui::PopStyleColor( );
				}

				ImGui::EndGroup( );
				ImGui::PopID( );

				return;
			}
			else
			{
				if ( asset )
				{
					Editor::Gui::SetSelectedAsset( asset );
				}
			}
		}

		if ( !selected )
		{
			ImGui::PopStyleColor( );
		}

		if ( asset != nullptr && ImGui::BeginDragDropSource( ImGuiDragDropFlags_SourceAllowNullID ) )
		{
			auto ptr = std::uintptr_t( asset );
			
			ImGui::SetDragDropPayload( "_ASSET", &ptr, sizeof(std::uintptr_t) );

			ImGui::Image( reinterpret_cast< ImTextureID >( icon ), ImVec2( 48.f, 48.f ), invertedUvs ? ImVec2( 1.f, 1.f ) : ImVec2( 0.f, 0.f ), invertedUvs ? ImVec2( 0.f, 0.f ) : ImVec2( 1.f, 1.f ) );

			ImGui::SameLine( );
			
			ImGui::Text( "%s", item->name.c_str(  ) );

			ImGui::EndDragDropSource( );
		}
		
		ImGui::Text( item->name.c_str( ) );

		ImGui::EndGroup( );
		ImGui::PopID( );

		ImGui::NextColumn( );
	}


}

Pine::IAsset* Gui::Widgets::AssetBrowser( Pine::IAsset* selectedAsset, Pine::EAssetType filter /*= Pine::EAssetType::Invalid*/ ) {

	root = Editor::DirectoryCache::GetRootDirectoryCache( );

	if ( currentPath == nullptr )
	{
		currentPath = root;
	}

	const bool inRootDirectory = root == currentPath;

	ImGui::BeginChild( "##AssetBrowserArea", ImVec2( -1.f, -1.f ), false );

	ImGui::Columns( 11, nullptr, false );

	for ( auto& item : currentPath->directories ) {
		DisplayItem( item.get( ) );
	}

	for ( auto& item : currentPath->files ) {
		DisplayItem( item.get( ) );
	}

	ImGui::Columns( 1 );

	ImGui::EndChild( );

	return nullptr;
}
