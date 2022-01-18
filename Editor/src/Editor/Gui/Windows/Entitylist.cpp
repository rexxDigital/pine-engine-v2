#include "Windows.hpp"
#include "../Gui.hpp"

#include <Pine/Pine.hpp>
#include <Pine/Entitylist/EntityList.hpp>

#include "Editor/EditorEntity/EditorEntity.hpp"
#include "Editor/Gui/Utility/HotkeyManager/HotkeyManager.hpp"
#include "Pine/Assets/Blueprint/Blueprint.hpp"

namespace
{

	// HACK: To fix context menus, since IsWindowHovered is out of control
	bool g_DidOpenContextMenu = false;
	bool g_OpenContextMenu = false;

	// HACK: For drag drop
	bool g_WasDragDroppingEntity = false;
	bool g_DidDropEntity = false;
	Pine::Entity* g_DroppedEntity = nullptr;

	// Since we might support selecting multiple entities in the future.
	bool IsSelectedEntity( Pine::Entity* e )
	{
		for ( const auto entity : Editor::Gui::Globals::SelectedEntityPtrs )
		{
			if ( entity == e )
			{
				return true;
			}
		}

		return false;
	}

	void SelectEntity( Pine::Entity* e )
	{
		if ( !ImGui::GetIO( ).KeyCtrl )
			Editor::Gui::Globals::SelectedEntityPtrs.clear( );

		Editor::Gui::Globals::SelectedEntityPtrs.push_back( e );
		Editor::Gui::Globals::SelectedAssetPtrs.clear( );

		Editor::Gui::Globals::SelectedItem = Editor::Gui::SelectedItemType::Entity;
	}

	void RenderEntity( Pine::Entity* e, bool isDragDroppingEntity )
	{
		const std::string renderText = e->GetName( ) + "##" + std::to_string( e->GetId( ) );
		const bool isSelectedEntity = IsSelectedEntity( e );

		auto flags = ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth;

		if ( isSelectedEntity )
			flags |= ImGuiTreeNodeFlags_Selected;
		if ( e->GetChildren( ).empty( ) )
			flags |= ImGuiTreeNodeFlags_Leaf;

		if ( ImGui::TreeNodeEx( renderText.c_str( ), flags ) )
		{
			if ( ImGui::IsItemClicked( ImGuiMouseButton_::ImGuiMouseButton_Right ) )
			{
				SelectEntity( e );

				g_OpenContextMenu = true;
				g_DidOpenContextMenu = true;
			}
			if ( ImGui::IsItemClicked( ) )
			{
				SelectEntity( e );
			}

			auto& children = e->GetChildren( );

			for ( auto& child : children )
			{
				RenderEntity( child, isDragDroppingEntity );
			}

			ImGui::TreePop( );
		}
		else
		{
			if ( ImGui::IsItemClicked( ) )
			{
				SelectEntity( e );
			}

			if ( ImGui::IsItemClicked( ImGuiMouseButton_::ImGuiMouseButton_Right ) )
			{
				SelectEntity( e );

				g_OpenContextMenu = true;
				g_DidOpenContextMenu = true;
			}
		}

		if ( ImGui::BeginDragDropTarget( ) )
		{
			if ( const auto payload = ImGui::AcceptDragDropPayload( "Entity", 0 ) )
			{
				const auto entity = *static_cast< Pine::Entity** >( payload->Data );
				e->AddChild( entity );
			}

			ImGui::EndDragDropTarget( );
		}

		ImGui::PopStyleVar( 2 );

		if ( ImGui::BeginDragDropSource( ImGuiDragDropFlags_::ImGuiDragDropFlags_None ) )
		{
			ImGui::SetDragDropPayload( "Entity", &e, sizeof( Pine::IAsset* ), 0 );

			ImGui::Text( e->GetName( ).c_str( ) );

			ImGui::EndDragDropSource( );
		}

		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.f, 2.f ) );
		ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 8.f, 0.f ) );
	}

	void EntityMoveSeparator( int entityIndex )
	{
		ImGui::PopStyleVar( 2 );

		const auto& mousePos = ImGui::GetMousePos( );
		const auto& avSize = ImGui::GetWindowSize( );
		const auto& windowDelta = ImGui::GetWindowPos( );

		// Couldn't use ImGui::GetWindowDrawList( ) for whatever reason.
		auto cursorPos = ImGui::GetCursorPos( );

		cursorPos.x += windowDelta.x;
		cursorPos.y += windowDelta.y;

		if ( fabsf( cursorPos.y - mousePos.y ) < 8.f )
		{
			ImGui::GetForegroundDrawList( )->AddRect( ImVec2( cursorPos.x, cursorPos.y - 4.f ), ImVec2( cursorPos.x + avSize.x - 1.f, cursorPos.y + 4.f ), ImColor( 255, 255, 0 ) );

			if ( g_DidDropEntity )
			{
				const auto entity = g_DroppedEntity;

				if ( entityIndex >= Pine::EntityList->GetEntities( ).size( ) )
					entityIndex = Pine::EntityList->GetEntities( ).size( ) - 1;

				Pine::EntityList->MoveEntity( entity, entityIndex );
			}
		}

		ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.f, 2.f ) );
		ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 8.f, 0.f ) );
	}

}

void Editor::Gui::Windows::RenderEntitylist( )
{
	if ( !ShowEntitylist )
		return;

	ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.f, 2.f ) );
	ImGui::PushStyleVar( ImGuiStyleVar_ItemSpacing, ImVec2( 8.f, 0.f ) );

	ImGui::Begin( "Entity list", &ShowEntitylist );

	auto& entities = Pine::EntityList->GetEntities( );

	bool isDragDroppingEntity = false;

	if ( const auto payload = ImGui::GetDragDropPayload( ) )
	{
		if ( std::string( payload->DataType ).find( "Entity" ) != std::string::npos )
		{
			isDragDroppingEntity = true;
		}
	}

	// We'll have to hack a bit more since I can't use BeginDragDropTarget, since we're not drag dropping on any ImGui widget.
	if ( isDragDroppingEntity )
	{
		g_WasDragDroppingEntity = true;
		g_DroppedEntity = *static_cast< Pine::Entity** >( ImGui::GetDragDropPayload( )->Data );
	}
	else
	{
		if ( g_WasDragDroppingEntity )
		{
			isDragDroppingEntity = true;

			g_WasDragDroppingEntity = false;
			g_DidDropEntity = true;
		}
	}

	if ( isDragDroppingEntity )
		EntityMoveSeparator( 1 ); // since editor entity is always 0, we begin at 1

	int entityRenderIndex = 0;

	for ( auto& entity : entities )
	{
		entityRenderIndex++;

		// Since we render children for each parent entity instead.
		if ( entity.GetParent( ) != nullptr )
		{
			continue;
		}

		if ( &entity == EditorEntity::GetEntity( ) )
		{
			continue;
		}

		RenderEntity( Pine::EntityList->GetEntity( entityRenderIndex - 1 ), isDragDroppingEntity );

		if ( isDragDroppingEntity )
			EntityMoveSeparator( entityRenderIndex );
	}

	g_DidDropEntity = false;

	ImGui::PopStyleVar( 2 );

	if ( ImGui::IsMouseClicked( ImGuiMouseButton_::ImGuiMouseButton_Left ) && ImGui::IsWindowHovered( ) )
	{
		Globals::SelectedEntityPtrs.clear( );
		Globals::SelectedAssetPtrs.clear( );
	}

	if ( g_OpenContextMenu )
	{
		ImGui::OpenPopup( "EntityContextMenu" );

		g_OpenContextMenu = false;
	}

	if ( ImGui::IsMouseClicked( ImGuiMouseButton_::ImGuiMouseButton_Right ) && ImGui::IsWindowHovered( ) && !g_DidOpenContextMenu )
	{
		Globals::SelectedEntityPtrs.clear( );
		Globals::SelectedAssetPtrs.clear( );

		ImGui::OpenPopup( "EntityContextMenu" );
	}

	if ( Globals::SelectedEntityPtrs.size( ) == 1 )
	{
		Pine::Entity* e = Globals::SelectedEntityPtrs[ 0 ];

		if ( HotkeyManager::GetHotkeyPressed( Hotkeys::RemoveEntityKey ) )
		{
			Pine::EntityList->DeleteEntity( e );
			Globals::SelectedEntityPtrs.clear( );
		}

		if ( HotkeyManager::GetHotkeyPressed( Hotkeys::DuplicateEntity ) )
		{
			Pine::Blueprint blueprint;

			blueprint.CreateFromEntity( e );

			const auto newEntity = blueprint.SpawnEntity( );

			blueprint.Dispose( );

			// We'll have to manually assign the parent as the blueprint won't be able to do that, as it wouldn't make any sense.
			if ( e->GetParent( ) != nullptr )
			{
				e->GetParent( )->AddChild( newEntity );
			}
		}
	}

	if ( ImGui::BeginPopup( "EntityContextMenu", 0 ) )
	{
		const bool isTargetingEntity = Editor::Gui::Globals::SelectedEntityPtrs.size( ) == 1;

		g_DidOpenContextMenu = false;

		Pine::Entity* e = nullptr;
		if ( isTargetingEntity )
			e = Globals::SelectedEntityPtrs[ 0 ];

		if ( ImGui::MenuItem( "Remove", nullptr, false, isTargetingEntity ) )
		{
			Pine::EntityList->DeleteEntity( e );
			Globals::SelectedEntityPtrs.clear( );

			ImGui::CloseCurrentPopup( );
		}

		if ( ImGui::MenuItem( "Duplicate", nullptr, false, isTargetingEntity ) )
		{
			Pine::Blueprint blueprint;

			blueprint.CreateFromEntity( e );
			blueprint.SpawnEntity( );

			blueprint.Dispose( );

			ImGui::CloseCurrentPopup( );
		}

		if ( ImGui::MenuItem( "Create a child", nullptr, false, isTargetingEntity ) )
		{
			e->CreateChild( );
			ImGui::CloseCurrentPopup( );
		}

		if ( ImGui::MenuItem( "Unlink from parent", nullptr, false, isTargetingEntity && e != nullptr && e->GetParent( ) != nullptr ) )
		{
			e->GetParent( )->RemoveChild( e );

			ImGui::CloseCurrentPopup( );
		}

		ImGui::Separator( );

		if ( ImGui::BeginMenu( "Create entity" ) )
		{
			if ( ImGui::MenuItem( "Empty" ) )
			{
				Pine::EntityList->CreateEntity( );
				ImGui::CloseCurrentPopup( );
			}

			if ( ImGui::MenuItem( "Model Renderer" ) )
			{
				auto e = Pine::EntityList->CreateEntity( );

				e->AddComponent( Pine::ComponentType::ModelRenderer );

				ImGui::CloseCurrentPopup( );
			}

			if ( ImGui::MenuItem( "Light" ) )
			{
				auto e = Pine::EntityList->CreateEntity( );

				e->AddComponent( Pine::ComponentType::Light );

				ImGui::CloseCurrentPopup( );
			}

			if ( ImGui::MenuItem( "Camera" ) )
			{
				auto e = Pine::EntityList->CreateEntity( );

				e->AddComponent( Pine::ComponentType::Camera );

				ImGui::CloseCurrentPopup( );
			}

			ImGui::EndMenu( );
		}

		ImGui::EndPopup( );
	}

	ImGui::End( );

}
