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
#include "Pine/Assets/Terrain/Terrain.hpp"
#include "Pine/Core/String/String.hpp"
#include "Pine/ScriptManager/ScriptManager.hpp"

namespace {

	int g_CreateType = 0;

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

	bool g_DidOpenContextMenu = false;
	PathItem_t* g_SelectedContextMenuItem = nullptr;

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
				// Ignore ".asset" files
				if ( Pine::String::EndsWith( dirEntry.path( ).string( ), ".asset" ) )
					continue;

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

			if ( Editor::Gui::Widgets::Icon( directory->m_DisplayText, g_SelectedContextMenuItem == directory.get( ), directoryIcon, 48, nullptr ) ) {
				if ( directory->m_DisplayText == "..." ) {
					g_CurrentDirectory = directory->m_Parent;
				}
				else {
					g_CurrentDirectory = directory.get( );
				}
			}

			if ( ImGui::IsItemClicked( ImGuiMouseButton_Right ) ) {
				ImGui::OpenPopup( "AssetContextMenu" );

				Editor::Gui::Globals::SelectedEntityPtrs.clear( );
				Editor::Gui::Globals::SelectedAssetPtrs.clear( );

				g_DidOpenContextMenu = true;
				g_SelectedContextMenuItem = directory.get( );
			}

			ImGui::NextColumn( );
		}

		// Process files:
		for ( auto& file : dir->m_Items ) {
			if ( file->m_IsDirectory )
				continue;

			auto icon = file->m_DisplayIcon != nullptr ? file->m_DisplayIcon : unknownFileIcon;

			if ( Editor::Gui::Widgets::Icon( file->m_DisplayText, selectedAsset != nullptr && selectedAsset == file->m_Asset, icon, 48, file->m_Asset ) ) {
				if ( file->m_Asset != nullptr ) {
					Editor::Gui::Globals::SelectedAssetPtrs.clear( );
					Editor::Gui::Globals::SelectedEntityPtrs.clear( );

					Editor::Gui::Globals::SelectedAssetPtrs.push_back( file->m_Asset );
				}
			}

			if ( ImGui::IsItemClicked( ImGuiMouseButton_Right ) ) {
				ImGui::OpenPopup( "AssetContextMenu" );

				Editor::Gui::Globals::SelectedEntityPtrs.clear( );
				Editor::Gui::Globals::SelectedAssetPtrs.clear( );
				Editor::Gui::Globals::SelectedAssetPtrs.push_back( file->m_Asset );

				g_DidOpenContextMenu = true;
				g_SelectedContextMenuItem = file.get( );
			}

			ImGui::NextColumn( );
		}
	}

}

void UpdateAssetCache( ) {
	if ( g_RootDirectory )
		g_RootDirectory.release( );

	g_RootDirectory = std::make_unique<PathItem_t>( );

	g_RootDirectory->m_IsDirectory = true;
	g_RootDirectory->m_Path = "";

	MapDirectory( "Engine", "Assets\\Engine", g_RootDirectory.get( ) );

	Editor::Gui::Utility::AssetIconGen::Update( );

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
		UpdateAssetCache( );
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

		if ( ImGui::IsMouseClicked( ImGuiMouseButton_::ImGuiMouseButton_Right ) && ImGui::IsWindowHovered( ) && !g_DidOpenContextMenu ) {
			Editor::Gui::Globals::SelectedEntityPtrs.clear( );
			Editor::Gui::Globals::SelectedAssetPtrs.clear( );
			g_SelectedContextMenuItem = nullptr;

			ImGui::OpenPopup( "AssetContextMenu" );
		}
	}

	// Since you can't open a pop up within another pop up
	bool openRenamePopup = false;
	bool openCreatePopup = false;

	if ( ImGui::BeginPopup( "AssetContextMenu", 0 ) ) {
		const bool isTargetingAsset = Editor::Gui::Globals::SelectedAssetPtrs.size( ) == 1;
		const bool isTargetingDirectory = g_SelectedContextMenuItem && g_SelectedContextMenuItem->m_IsDirectory;

		g_DidOpenContextMenu = false;

		if ( ImGui::BeginMenu( "Create" ) ) {
			if ( ImGui::MenuItem( "Directory", nullptr ) ) {
				g_CreateType = 0;
				openCreatePopup = true;
			}

			if ( ImGui::MenuItem( "Material", nullptr ) ) {
				g_CreateType = 1;
				openCreatePopup = true;
			}

			if ( ImGui::MenuItem( "Level", nullptr ) ) {
				g_CreateType = 2;
				openCreatePopup = true;
			}

			if ( ImGui::MenuItem( "Terrain", nullptr ) ) {
				g_CreateType = 3;
				openCreatePopup = true;
			}

			ImGui::EndMenu( );
		}

		ImGui::Separator( );

		if ( ImGui::MenuItem( "Rename", "F2", false, isTargetingAsset || isTargetingDirectory ) ) {
			ImGui::CloseCurrentPopup( );
			openRenamePopup = true;
		}

		if ( ImGui::MenuItem( "Remove", "DEL", false, isTargetingAsset || isTargetingDirectory ) ) {

			if ( isTargetingAsset ) {
				auto asset = Editor::Gui::Globals::SelectedAssetPtrs[ 0 ];

				if ( !asset->GetReadOnly( ) ) {
					std::filesystem::remove( asset->GetPath( ) );

					Pine::Assets::DisposeAsset( asset );

					UpdateAssetCache( );
					ProjectManager::ReloadProjectAssets( );

					Editor::Gui::Globals::SelectedAssetPtrs.clear( );

					g_SelectedContextMenuItem = nullptr;
				}
			}

			if ( isTargetingDirectory ) {
				std::filesystem::remove( g_SelectedContextMenuItem->m_Path );

				UpdateAssetCache( );
				ProjectManager::ReloadProjectAssets( );

				Editor::Gui::Globals::SelectedAssetPtrs.clear( );

				g_SelectedContextMenuItem = nullptr;
			}

			Editor::Gui::Globals::SelectedEntityPtrs.clear( );
			Editor::Gui::Globals::SelectedAssetPtrs.clear( );
			g_SelectedContextMenuItem = nullptr;

			ImGui::CloseCurrentPopup( );
		}

		ImGui::EndPopup( );
	}
	else {
		g_SelectedContextMenuItem = nullptr;
	}

	if ( openRenamePopup ) {
		ImGui::OpenPopup( "Rename" );
	}

	if ( openCreatePopup ) {
		ImGui::OpenPopup( "Create Item" );
	}

	// Show Rename Popup

	static bool renamePopupOpened = false;
	if ( ImGui::BeginPopupModal( "Rename", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize ) ) {
		static char buff[ 128 ];

		if ( renamePopupOpened ) {
			renamePopupOpened = false;

			strcpy_s( buff, Editor::Gui::Globals::SelectedAssetPtrs[ 0 ]->GetFileName( ).c_str( ) );

			ImGui::SetKeyboardFocusHere( 0 );
		}

		ImGui::Text( "Name:" );
		ImGui::InputText( "##NewName", buff, 128 );

		if ( ImGui::Button( "OK" ) ) {
			auto asset = Editor::Gui::Globals::SelectedAssetPtrs[ 0 ];

			if ( !asset->GetReadOnly( ) ) {
				std::filesystem::rename( asset->GetPath( ), std::string( asset->GetPath( ).parent_path( ).string( ) + "\\" + buff ) );

				Pine::Assets::DisposeAsset( asset );

				UpdateAssetCache( );
				ProjectManager::ReloadProjectAssets( );

				Editor::Gui::Globals::SelectedEntityPtrs.clear( );
				Editor::Gui::Globals::SelectedAssetPtrs.clear( );
				g_SelectedContextMenuItem = nullptr;
			}

			ImGui::CloseCurrentPopup( );
		}

		ImGui::SameLine( );

		if ( ImGui::Button( "Cancel" ) ) {
			ImGui::CloseCurrentPopup( );
		}

		ImGui::EndPopup( );
	}
	else {
		renamePopupOpened = true;
	}

	// Show create asset pop up

	static bool createPopupOpened = false;
	if ( ImGui::BeginPopupModal( "Create Item", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize ) ) {
		static char buff[ 128 ];

		if ( createPopupOpened ) {
			createPopupOpened = false;

			strcpy_s( buff, "" );

			ImGui::SetKeyboardFocusHere( 0 );
		}

		ImGui::Text( "Name:" );
		ImGui::InputText( "##NewName", buff, 128 );

		if ( ImGui::Button( "OK" ) ) {
			std::string baseDir = g_CurrentDirectory->m_Path.string( );

			if ( g_CurrentDirectory == g_RootDirectory.get( ) )
				baseDir = ProjectManager::GetCurrentProjectDirectory( );

			// Directory
			if ( g_CreateType == 0 ) {
				std::filesystem::create_directory( std::string( baseDir + "\\" + buff ) );

				ProjectManager::ReloadProjectAssets( );
				UpdateAssetCache( );
			}

			// Material
			if ( g_CreateType == 1 ) {
				std::filesystem::copy( "Assets\\Engine\\Materials\\Default.mat", baseDir + "\\" + buff + ".mat" );

				ProjectManager::ReloadProjectAssets( );
				UpdateAssetCache( );
			}

			// Level
			if ( g_CreateType == 2 ) {
				ProjectManager::SaveLevel( baseDir + "\\" + buff + ".lvl" );
				ProjectManager::ReloadProjectAssets( );
				UpdateAssetCache( );
			}

			// Terrain
			if ( g_CreateType == 3 ) {
				Pine::Terrain* terrain = new Pine::Terrain( );

				terrain->SetFilePath( baseDir + "\\" + buff + ".ter" );
				terrain->SaveToFile( );
				terrain->Dispose( );
				
				delete terrain;

				ProjectManager::ReloadProjectAssets( );
				UpdateAssetCache( );
			}
			
			ImGui::CloseCurrentPopup( );
		}

		ImGui::SameLine( );

		if ( ImGui::Button( "Cancel" ) ) {
			ImGui::CloseCurrentPopup( );
		}

		ImGui::EndPopup( );
	}
	else {
		createPopupOpened = true;
	}

	ImGui::EndChild( );

	ImGui::End( );

}