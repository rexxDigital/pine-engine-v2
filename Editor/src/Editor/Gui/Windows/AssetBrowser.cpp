#include "Windows.hpp"
#include <filesystem>
#include <string>
#include <memory>
#include <Pine\Assets\IAsset\IAsset.hpp>
#include "Pine\Assets\Assets.hpp"
#include "..\..\ProjectManager\ProjectManager.hpp"
#include <Pine\Assets\Texture2D\Texture2D.hpp>
#include "..\Widgets\Widgets.hpp"
#include "..\Gui.hpp"
#include "..\Utility\AssetIconGen\AssetIconGen.hpp"

namespace {

	// Could be a file or a directory
	struct PathItem_t {
		bool m_IsDirectory = false;

		Pine::IAsset* m_Asset = nullptr;

		std::filesystem::path m_Path;

		std::string m_DisplayText = "";
		Pine::Texture2D* m_DisplayIcon = nullptr;

		std::vector<std::unique_ptr<PathItem_t>> m_Items;
		PathItem_t* m_Parent = nullptr;
	};

	void ProcessDirectory( const std::string& dir, PathItem_t* item ) {
		// Add a "..." go back directory.
		if ( item->m_Parent != nullptr ) {
			auto entry = std::make_unique<PathItem_t>( );

			entry->m_IsDirectory = true;
			entry->m_Path = item->m_Parent->m_Path;
			entry->m_Parent = item->m_Parent;
			entry->m_DisplayText = "...";

			item->m_Items.push_back( std::move( entry ) );
		}

		for ( const auto& dirEntry : std::filesystem::directory_iterator( dir ) ) {
			if ( dirEntry.is_directory( ) ) { // Process directory:
				auto entry = std::make_unique<PathItem_t>( );

				entry->m_IsDirectory = true;
				entry->m_Path = dirEntry.path( );
				entry->m_Parent = item;
				entry->m_DisplayText = dirEntry.path( ).filename( ).string( );

				ProcessDirectory( entry->m_Path.string( ), entry.get( ) );

				item->m_Items.push_back( std::move( entry ) );
			}
			else { // Process file:
				auto entry = std::make_unique<PathItem_t>( );

				entry->m_Path = dirEntry.path( );
				entry->m_Parent = item;
				entry->m_Asset = Pine::Assets::GetAsset( entry->m_Path.string( ) );
				entry->m_DisplayIcon = Editor::Gui::Utility::AssetIconGen::GetAssetIcon( entry->m_Path.string( ) );
				entry->m_DisplayText = dirEntry.path( ).filename( ).string( );

				item->m_Items.push_back( std::move( entry ) );
			}
		}
	}

	void MapDirectory( const std::string& displayDirectory, const std::string& dir, PathItem_t* item ) {
		auto entry = std::make_unique<PathItem_t>( );

		entry->m_IsDirectory = true;
		entry->m_Path = displayDirectory;
		entry->m_Parent = item;
		entry->m_DisplayText = displayDirectory;

		ProcessDirectory( dir, entry.get( ) );

		item->m_Items.push_back( std::move( entry ) );
	}

	std::unique_ptr<PathItem_t> g_RootDirectory = nullptr;

	constexpr int IconSize = 48;

	PathItem_t* g_CurrentDirectory = nullptr;

	void DisplayItems( PathItem_t* dir ) {
		static auto directoryIcon = Pine::Assets::GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\folder.png" );
		static auto unknownFileIcon = Pine::Assets::GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\corrupt.png" );

		Pine::IAsset* selectedAsset = nullptr;

		if ( !Editor::Gui::Globals::SelectedAssetPtrs.empty( ) ) {
			selectedAsset = Editor::Gui::Globals::SelectedAssetPtrs[ 0 ];
		}

		// Process directories:
		for ( auto& directory : dir->m_Items ) {
			if ( !directory->m_IsDirectory )
				continue;

			if ( Editor::Gui::Widgets::Icon( directory->m_DisplayText, false, directoryIcon, 48 ) ) {
				if ( directory->m_DisplayText == "..." ) {
					g_CurrentDirectory = directory->m_Parent;
				}
				else {
					g_CurrentDirectory = directory.get( );
				}
			}

			ImGui::NextColumn( );
		}

		// Process files:
		for ( auto& file : dir->m_Items ) {
			if ( file->m_IsDirectory )
				continue;

			auto icon = file->m_DisplayIcon != nullptr ? file->m_DisplayIcon : unknownFileIcon;

			if ( Editor::Gui::Widgets::Icon( file->m_DisplayText, selectedAsset != nullptr && selectedAsset == file->m_Asset, icon, 48 ) ) {
				if ( file->m_Asset != nullptr ) {
					Editor::Gui::Globals::SelectedAssetPtrs.clear( );
					Editor::Gui::Globals::SelectedEntityPtrs.clear( );

					Editor::Gui::Globals::SelectedAssetPtrs.push_back( file->m_Asset );
				}
			}

			ImGui::NextColumn( );
		}
	}

}

void UpdateAssetCache( ) {
	g_RootDirectory = std::make_unique<PathItem_t>( );

	g_RootDirectory->m_IsDirectory = true;
	g_RootDirectory->m_Path = "";

	MapDirectory( "Engine", "Assets\\Engine", g_RootDirectory.get( ) );

	ProcessDirectory( Editor::ProjectManager::GetCurrentProjectDirectory( ), g_RootDirectory.get( ) );

	g_CurrentDirectory = g_RootDirectory.get( );
}

void Editor::Gui::Windows::RenderAssetBrowser( ) {

	if ( !ShowAssetBrowser ) {
		return;
	}

	ImGui::Begin( "Asset Browser", &ShowAssetBrowser, 0 );

	if ( ImGui::Button( "Refresh project assets" ) ) {
		ProjectManager::ReloadProjectAssets( );
	}

	ImGui::Separator( );

	ImGui::BeginChild( "##Assets", ImVec2( -1.f, -1.f ), true, 0 );

	constexpr int iconSizePadding = IconSize + 16;

	const float spaceAvailable = ImGui::GetContentRegionAvail( ).x - ( iconSizePadding * 2 );
	const int nrColumns = static_cast< int >( spaceAvailable ) / iconSizePadding;

	if ( nrColumns > 0 ) {
		ImGui::Columns( nrColumns, 0, false );

		DisplayItems( g_CurrentDirectory );

		ImGui::Columns( 1 );

		if ( ImGui::IsMouseClicked( ImGuiMouseButton_::ImGuiMouseButton_Left ) && ImGui::IsWindowHovered( ) ) {
			Editor::Gui::Globals::SelectedAssetPtrs.clear( );
			Editor::Gui::Globals::SelectedEntityPtrs.clear( );
		}
	}

	ImGui::EndChild( );

	ImGui::End( );

}