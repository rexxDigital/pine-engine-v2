#include "Windows.hpp"
#include <filesystem>
#include <string>
#include <memory>
#include <Pine/Assets/IAsset/IAsset.hpp>
#include <Pine/Assets/Texture2D/Texture2D.hpp>
#include "Pine/Assets/Assets.hpp"
#include "Pine/Assets/Terrain/Terrain.hpp"
#include "Pine/Core/Log/Log.hpp"
#include "Pine/Core/String/String.hpp"
#include "../../ProjectManager/ProjectManager.hpp"
#include "../Widgets/Widgets.hpp"
#include "../Gui.hpp"
#include "../Utility/AssetIcon/AssetIcon.hpp"
#include "Pine/Assets/Texture3D/Texture3D.hpp"
#include "Pine/Entity/Entity.hpp"
#include "Editor/Gui/Utility/HotkeyManager/HotkeyManager.hpp"

// Could be a file or a directory
struct PathItem_t
{
    bool m_IsDirectory = false;

    Pine::IAsset* m_Asset = nullptr;

    std::filesystem::path m_Path;

    std::string m_DisplayText = "";
    Editor::Gui::Utility::AssetIcon::AssetIcon_t* m_DisplayIcon = nullptr;

    std::vector<PathItem_t*> m_Items;
    PathItem_t* m_Parent = nullptr;

    void Dispose( ) const
    {
        for ( auto item : m_Items )
        {
            item->Dispose( );
            delete item;
        }
    }
};

namespace
{

	int g_CreateType = 0;

	bool g_DidOpenContextMenu = false;
	PathItem_t* g_SelectedContextMenuItem = nullptr;

	void ProcessDirectory( const std::string& dir, PathItem_t* item )
	{
		// Add a "..." go back directory.
		if ( item->m_Parent != nullptr )
		{
			auto entry = new PathItem_t;

			entry->m_IsDirectory = true;
			entry->m_Path = item->m_Parent->m_Path;
			entry->m_Parent = item->m_Parent;
			entry->m_DisplayText = "...";

			item->m_Items.push_back( entry );
		}

		for ( const auto& dirEntry : std::filesystem::directory_iterator( dir ) )
		{
			if ( dirEntry.is_directory( ) ) // Process directory:
			{ 
				auto entry = new PathItem_t;

				entry->m_IsDirectory = true;
				entry->m_Path = dirEntry.path( );
				entry->m_Parent = item;
				entry->m_DisplayText = dirEntry.path( ).filename( ).string( );

				ProcessDirectory( entry->m_Path.string( ), entry );

				item->m_Items.push_back( entry );
			}
			else // Process file:
			{ 
				// Ignore ".asset" files
				if ( Pine::String::EndsWith( dirEntry.path( ).string( ), ".asset" ) )
					continue;

				auto entry = new PathItem_t;

				entry->m_Path = dirEntry.path( );
				entry->m_Parent = item;
				entry->m_Asset = Pine::Assets->GetAsset( entry->m_Path.string( ) );
				entry->m_DisplayIcon = Editor::Gui::Utility::AssetIcon::GetAssetIcon( entry->m_Path.string( ) );
				entry->m_DisplayText = dirEntry.path( ).filename( ).string( );

				item->m_Items.push_back( entry );
			}
		}
	}

	void MapDirectory( const std::string& displayDirectory, const std::string& dir, PathItem_t* item )
	{
		auto entry = new PathItem_t;

		entry->m_IsDirectory = true;
		entry->m_Path = dir;
		entry->m_Parent = item;
		entry->m_DisplayText = displayDirectory;

		ProcessDirectory( dir, entry );

		item->m_Items.push_back( entry );
	}

	PathItem_t* g_RootDirectory = nullptr;
	PathItem_t* g_CurrentDirectory = nullptr;

	int g_IconSize = 64;

	void DisplayItems( PathItem_t* dir )
	{
		static auto directoryIcon = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets/Editor/Icons/folder.png" );
		static auto unknownFileIcon = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets/Editor/Icons/corrupt.png" );

		Pine::IAsset* selectedAsset = nullptr;

		if ( !Editor::Gui::Globals::SelectedAssetPtrs.empty( ) )
		{
			selectedAsset = Editor::Gui::Globals::SelectedAssetPtrs[ 0 ];
		}

		// Process directories:
		for ( auto& directory : dir->m_Items )
		{
			if ( !directory->m_IsDirectory )
				continue;

			if ( Editor::Gui::Widgets::Icon( directory->m_DisplayText, g_SelectedContextMenuItem == directory, directoryIcon, g_IconSize, nullptr ) )
			{
				if ( directory->m_DisplayText == "..." )
				{
					g_CurrentDirectory = directory->m_Parent;
				}
				else
				{
					g_CurrentDirectory = directory;
				}
			}

			if ( ImGui::IsItemClicked( ImGuiMouseButton_Right ) )
			{
				ImGui::OpenPopup( "AssetContextMenu" );

				Editor::Gui::Globals::SelectedEntityPtrs.clear( );
				Editor::Gui::Globals::SelectedAssetPtrs.clear( );

				g_DidOpenContextMenu = true;
				g_SelectedContextMenuItem = directory;
			}

            bool exitLoop = false;

			if ( ImGui::BeginDragDropTarget( ) )
			{
				if ( const auto payload = ImGui::AcceptDragDropPayload( "Asset", 0 ) )
				{
                    Pine::IAsset* asset = *reinterpret_cast<Pine::IAsset**>(payload->Data);

                    // Copy old asset and then remove it.
                    std::filesystem::copy( asset->GetPath( ), directory->m_Path.string( ) + "/" + asset->GetFileName( ) );

                    std::filesystem::remove( asset->GetPath( ) );

                    Pine::Assets->DisposeAsset( asset );

                    Editor::ProjectManager::ReloadProjectAssets( );

                    exitLoop = true;
				}

                if ( const auto payload = ImGui::AcceptDragDropPayload( "Directory", 0 ) )
                {
                    // TODO: Add this feature.
                }

                ImGui::EndDragDropTarget( );
			}

            if ( exitLoop )
            {
                return;
            }

			ImGui::NextColumn( );
		}

		// Process files:
		for ( auto& file : dir->m_Items )
		{
			if ( file->m_IsDirectory )
				continue;

			Pine::Texture2D* icon;
            bool isDynamic = false;

			if ( file->m_DisplayIcon != nullptr )
			{
				if ( file->m_DisplayIcon->m_LiveIconReady && !file->m_DisplayIcon->m_LiveIconDirty )
				{
					icon = file->m_DisplayIcon->m_FrameBufferTexture;
                    isDynamic = true;
				}
				else
				{
					icon = file->m_DisplayIcon->m_Texture2D;
				}
			}
			else
			{
				icon = unknownFileIcon;
			}

			if ( Editor::Gui::Widgets::Icon( file->m_DisplayText, selectedAsset != nullptr && selectedAsset == file->m_Asset, icon, g_IconSize, file->m_Asset, nullptr, isDynamic ) )
			{
				if ( file->m_Asset != nullptr )
				{
					Editor::Gui::Globals::SelectedAssetPtrs.clear( );
					Editor::Gui::Globals::SelectedEntityPtrs.clear( );

					Editor::Gui::Globals::SelectedAssetPtrs.push_back( file->m_Asset );
				}
			}

			if ( ImGui::IsItemClicked( ImGuiMouseButton_Right ) )
			{
				ImGui::OpenPopup( "AssetContextMenu" );

				Editor::Gui::Globals::SelectedEntityPtrs.clear( );
				Editor::Gui::Globals::SelectedAssetPtrs.clear( );
				Editor::Gui::Globals::SelectedAssetPtrs.push_back( file->m_Asset );

				g_DidOpenContextMenu = true;
				g_SelectedContextMenuItem = file;
			}

			ImGui::NextColumn( );
		}
	}

	PathItem_t* FindDirectory( PathItem_t* root, const std::string& path )
	{
		if ( !root->m_IsDirectory )
			return nullptr;

		if ( root->m_Path.string( ) == path )
		{
			return root;
		}

		for ( const auto& item : root->m_Items )
		{
			if ( !item->m_IsDirectory )
				continue;
			if ( item->m_DisplayText == "..." ) // lol
				continue;

			if ( const auto ret = FindDirectory( item, path ) )
				return ret;
		}

		return nullptr;
	}

    void DeleteSelectedItem( )
    {
        const bool isTargetingAsset = Editor::Gui::Globals::SelectedAssetPtrs.size( ) == 1;
        const bool isTargetingDirectory = g_SelectedContextMenuItem && g_SelectedContextMenuItem->m_IsDirectory;

        if ( isTargetingAsset )
        {
            auto asset = Editor::Gui::Globals::SelectedAssetPtrs[ 0 ];

            if ( !asset->GetReadOnly( ) )
            {
                std::filesystem::remove( asset->GetPath( ) );

                Pine::Assets->DisposeAsset( asset );

                Editor::ProjectManager::ReloadProjectAssets( );

                Editor::Gui::Globals::SelectedAssetPtrs.clear( );

                g_SelectedContextMenuItem = nullptr;
            }
        }

        if ( isTargetingDirectory )
        {
            std::filesystem::remove( g_SelectedContextMenuItem->m_Path );

            Editor::ProjectManager::ReloadProjectAssets( );

            Editor::Gui::Globals::SelectedAssetPtrs.clear( );

            g_SelectedContextMenuItem = nullptr;
        }

        Editor::Gui::Globals::SelectedEntityPtrs.clear( );
        Editor::Gui::Globals::SelectedAssetPtrs.clear( );
        g_SelectedContextMenuItem = nullptr;
    }

}

void UpdateAssetCache( )
{
	std::string prevOpenDirectory = "";

	if ( g_RootDirectory )
	{
		if ( g_CurrentDirectory )
			prevOpenDirectory = g_CurrentDirectory->m_Path.string( );

		g_RootDirectory->Dispose( );
		delete g_RootDirectory;
	}

	g_RootDirectory = new PathItem_t;

	g_RootDirectory->m_IsDirectory = true;
	g_RootDirectory->m_Path = Editor::ProjectManager::GetCurrentProjectDirectory( );

	MapDirectory( "Engine", "Assets/Engine", g_RootDirectory );

	Editor::Gui::Utility::AssetIcon::Update( );

	ProcessDirectory( Editor::ProjectManager::GetCurrentProjectDirectory( ), g_RootDirectory );

	if ( !prevOpenDirectory.empty(  ) )
	{
		g_CurrentDirectory = FindDirectory( g_RootDirectory, prevOpenDirectory );

		if ( !g_CurrentDirectory )
		{
			g_CurrentDirectory = g_RootDirectory;
		}
	}
	else
	{
		g_CurrentDirectory = g_RootDirectory;
	}
}

void Editor::Gui::Windows::RenderAssetBrowser( )
{
	if ( !ShowAssetBrowser )
	{
		return;
	}

	if ( ImGui::Begin( "Asset Browser", &ShowAssetBrowser, 0 ) )
	{
		if ( ImGui::Button( "Import..." ) )
		{

		}

		ImGui::SameLine( );

		if ( ImGui::Button( "Refresh project assets" ) )
		{
			ProjectManager::ReloadProjectAssets( );
		}

		ImGui::SameLine( );

		if ( ImGui::Button( "Open in Explorer" ) )
		{
            // This is secure and safe, and good.
			system( std::string( "explorer.exe " + std::filesystem::absolute( g_CurrentDirectory->m_Path ).string( ) ).c_str(  ) );
		}

		ImGui::SameLine( );

		ImGui::SetCursorPosX( ImGui::GetWindowWidth( ) - 128.f - 10.f );
		ImGui::SetNextItemWidth( 128.f );
		ImGui::InputInt( "##IconSize", &g_IconSize, 4, 16, 0 );

		ImGui::BeginChild( "##Assets", ImVec2( -1.f, -1.f ), true, 0 );

		const int iconSizePadding = g_IconSize + 16;

		const float spaceAvailable = ImGui::GetContentRegionAvail( ).x - ( iconSizePadding * 2 );
		const int nrColumns = static_cast< int >( spaceAvailable ) / iconSizePadding;

		if ( nrColumns > 0 && g_CurrentDirectory != nullptr )
		{
			ImGui::Columns( nrColumns, 0, false );

			DisplayItems( g_CurrentDirectory );

			ImGui::Columns( 1 );

			if ( ImGui::IsMouseClicked( ImGuiMouseButton_::ImGuiMouseButton_Left ) && ImGui::IsWindowHovered( ) )
			{
				Editor::Gui::Globals::SelectedAssetPtrs.clear( );
				Editor::Gui::Globals::SelectedEntityPtrs.clear( );
			}

			if ( ImGui::IsMouseClicked( ImGuiMouseButton_::ImGuiMouseButton_Right ) && ImGui::IsWindowHovered( ) && !g_DidOpenContextMenu )
			{
				Editor::Gui::Globals::SelectedEntityPtrs.clear( );
				Editor::Gui::Globals::SelectedAssetPtrs.clear( );
				g_SelectedContextMenuItem = nullptr;

				ImGui::OpenPopup( "AssetContextMenu" );
			}
		}

		// Since you can't open a pop up within another pop up
		bool openRenamePopup = false;
		bool openCreatePopup = false;

		if ( ImGui::BeginPopup( "AssetContextMenu", 0 ) )
		{
			const bool isTargetingAsset = Editor::Gui::Globals::SelectedAssetPtrs.size( ) == 1;
			const bool isTargetingDirectory = g_SelectedContextMenuItem && g_SelectedContextMenuItem->m_IsDirectory;

			g_DidOpenContextMenu = false;

			if ( ImGui::BeginMenu( "Create" ) )
			{
				if ( ImGui::MenuItem( "Directory", nullptr ) )
				{
					g_CreateType = 0;
					openCreatePopup = true;
				}

				if ( ImGui::MenuItem( "Material", nullptr ) )
				{
					g_CreateType = 1;
					openCreatePopup = true;
				}

				if ( ImGui::MenuItem( "Level", nullptr ) )
				{
					g_CreateType = 2;
					openCreatePopup = true;
				}

				if ( ImGui::MenuItem( "Terrain", nullptr ) )
				{
					g_CreateType = 3;
					openCreatePopup = true;
				}

				if ( ImGui::MenuItem( "Texture3D", nullptr ) )
				{
					g_CreateType = 4;
					openCreatePopup = true;
				}

				ImGui::EndMenu( );
			}

			ImGui::Separator( );

			if ( ImGui::MenuItem( "Rename", "F2", false, isTargetingAsset || isTargetingDirectory ) )
			{
				ImGui::CloseCurrentPopup( );
				openRenamePopup = true;
			}

			if ( ImGui::MenuItem( "Remove", "DEL", false, isTargetingAsset || isTargetingDirectory ) )
			{
                DeleteSelectedItem( );

				ImGui::CloseCurrentPopup( );
			}

			ImGui::EndPopup( );
		}
		else
		{
			g_SelectedContextMenuItem = nullptr;
		}

        if ( HotkeyManager::GetHotkeyPressed( Hotkeys::DeleteKey ) )
        {
            DeleteSelectedItem( );
        }

        if ( HotkeyManager::GetHotkeyPressed( Hotkeys::RenameKey ) )
        {
            openRenamePopup = true;
        }

		if ( openRenamePopup )
		{
			ImGui::OpenPopup( "Rename" );
		}

		if ( openCreatePopup )
		{
			ImGui::OpenPopup( "Create Item" );
		}

		// Show Rename Popup

		static bool renamePopupOpened = false;
		if ( ImGui::BeginPopupModal( "Rename", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize ) )
		{
			static char buff[ 128 ];

			if ( renamePopupOpened )
			{
				renamePopupOpened = false;

				strcpy( buff, Editor::Gui::Globals::SelectedAssetPtrs[ 0 ]->GetFileName( ).c_str( ) );

				ImGui::SetKeyboardFocusHere( 0 );
			}

			ImGui::Text( "Name:" );
			ImGui::InputText( "##NewName", buff, 128 );

			if ( ImGui::Button( "OK" ) )
			{
				auto asset = Editor::Gui::Globals::SelectedAssetPtrs[ 0 ];

				if ( !asset->GetReadOnly( ) )
				{
					std::filesystem::rename( asset->GetPath( ), std::string( asset->GetPath( ).parent_path( ).string( ) + "/" + buff ) );

					Pine::Assets->DisposeAsset( asset );

					ProjectManager::ReloadProjectAssets( );

					Editor::Gui::Globals::SelectedEntityPtrs.clear( );
					Editor::Gui::Globals::SelectedAssetPtrs.clear( );
					g_SelectedContextMenuItem = nullptr;
				}

				ImGui::CloseCurrentPopup( );
			}

			ImGui::SameLine( );

			if ( ImGui::Button( "Cancel" ) )
			{
				ImGui::CloseCurrentPopup( );
			}

			ImGui::EndPopup( );
		}
		else
		{
			renamePopupOpened = true;
		}

		// Show create asset pop up

		static bool createPopupOpened = false;
		if ( ImGui::BeginPopupModal( "Create Item", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize ) )
		{
			static char buff[ 128 ];

			if ( createPopupOpened )
			{
				createPopupOpened = false;

				strcpy( buff, "" );

				ImGui::SetKeyboardFocusHere( 0 );
			}

			ImGui::InputText( "Name", buff, 128 );
			 
			ImGui::Spacing( );
			ImGui::Spacing( );
			ImGui::Spacing( );

			if ( ImGui::Button( "OK" ) || ImGui::IsKeyPressed( 257 /* Enter */ ) )
			{
				std::string baseDir = g_CurrentDirectory->m_Path.string( );

				if ( g_CurrentDirectory == g_RootDirectory )
					baseDir = ProjectManager::GetCurrentProjectDirectory( );

				// Directory
				if ( g_CreateType == 0 )
				{
					std::filesystem::create_directory( std::string( baseDir + "/" + buff ) );

					ProjectManager::ReloadProjectAssets( );
				}

				// Material
				if ( g_CreateType == 1 )
				{
					std::filesystem::copy( "Assets/Engine/Materials/Default.mat", baseDir + "/" + buff + ".mat" );

					ProjectManager::ReloadProjectAssets( );
				}

				// Level
				if ( g_CreateType == 2 )
				{
					ProjectManager::SaveLevel( baseDir + "/" + buff + ".lvl" );
					ProjectManager::ReloadProjectAssets( );
				}

				// Terrain
				if ( g_CreateType == 3 )
				{
					const auto terrain = new Pine::Terrain( );

					terrain->SetFilePath( baseDir + "/" + buff + ".ter" );
					terrain->SaveToFile( );
					terrain->Dispose( );

					delete terrain;

					ProjectManager::ReloadProjectAssets( );
				}

				// Texture3D
				if ( g_CreateType == 4 )
				{
					const auto texture3D = new Pine::Texture3D( );

					texture3D->SetFilePath( baseDir + "/" + buff + ".cmap" );
					texture3D->SetUpdated( true );
					texture3D->SaveToFile( );
					texture3D->Dispose( );

					delete texture3D;

					ProjectManager::ReloadProjectAssets( );
				}

				ImGui::CloseCurrentPopup( );
			}

			ImGui::SameLine( );

			if ( ImGui::Button( "Cancel" ) )
			{
				ImGui::CloseCurrentPopup( );
			}

			ImGui::EndPopup( );
		}
		else
		{
			createPopupOpened = true;
		}

		ImGui::EndChild( );

		if ( ImGui::BeginDragDropTarget( ) )
		{
			if ( const auto payload = ImGui::AcceptDragDropPayload( "Entity", 0 ) )
			{
				const auto entity = *static_cast< Pine::Entity** >( payload->Data );

				// Create a prefab of the entity

				Pine::Blueprint blueprint;

				blueprint.CreateFromEntity( entity );

				// I don't feel so good about saving it with the entity's file name but whatever.
				blueprint.SetFilePath( g_CurrentDirectory->m_Path.string( ) + "/" + entity->GetName( ) + ".bpt" );

				blueprint.SaveToFile( );

				ProjectManager::ReloadProjectAssets( );
			}

			ImGui::EndDragDropTarget( );
		}

	}
	ImGui::End( );

}