#include "HotkeyManager.hpp"

#include <vector>
#include <thread>

#include "ImGui/imgui.h"

namespace
{

	struct HotkeyData
	{
		std::string m_Name = "";
		int m_Key = -1;
		bool m_Ctrl = false;
		bool m_Alt = false;
	};

	std::vector<HotkeyData> g_Hotkeys;

}

uint32_t Editor::Gui::HotkeyManager::RegisterHotkey( const std::string& name, int defaultKey, bool ctrl, bool alt )
{
	HotkeyData data;
	
	data.m_Name = name;
	data.m_Key = defaultKey;
	data.m_Ctrl = ctrl;
	data.m_Alt = alt;

	g_Hotkeys.push_back( data );
	
	return g_Hotkeys.size(  ) - 1;
}

bool Editor::Gui::HotkeyManager::GetHotkeyPressed( uint32_t hotkey )
{
	auto& data = g_Hotkeys[ hotkey ];
	auto& io = ImGui::GetIO( );

	if ( ImGui::IsMouseDown( ImGuiMouseButton_::ImGuiMouseButton_Right ) )
	{
		return false;
	}
	
	if ( ( data.m_Ctrl && !io.KeyCtrl ) || ( !data.m_Ctrl && io.KeyCtrl ) )
	{
		return false;
	}

	if ( ( data.m_Alt && !io.KeyAlt ) || ( !data.m_Alt && io.KeyAlt ) )
	{
		return false;
	}

	return ImGui::IsKeyPressed( data.m_Key );
}

namespace Editor::Hotkeys {

	PE_DEFINE_HOTKEY( TransformGizmo, GLFW_KEY_T, false, false );
	PE_DEFINE_HOTKEY( RotateGizmo, GLFW_KEY_R, false, false );
	PE_DEFINE_HOTKEY( ScaleGizmo, GLFW_KEY_S, false, false );

	PE_DEFINE_HOTKEY( SaveHotkey, GLFW_KEY_S, true, false );
	PE_DEFINE_HOTKEY( RefreshAssetsHotkey, GLFW_KEY_F5, false, false );

	PE_DEFINE_HOTKEY( RemoveEntityKey, GLFW_KEY_DELETE, false, false );
	PE_DEFINE_HOTKEY( DuplicateEntity, GLFW_KEY_D, true, false );

}