#include "HotkeyManager.hpp"
#include <vector>

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

	std::vector<HotkeyData> hotkeys;

}

uint32_t Editor::Gui::HotkeyManager::RegisterHotkey( const std::string& name, int defaultKey, bool ctrl, bool alt )
{
	HotkeyData data;

	data.m_Name = name;
	data.m_Key = defaultKey;
	data.m_Ctrl = ctrl;
	data.m_Alt = alt;

	hotkeys.push_back( data );

	return hotkeys.size( ) - 1;
}

bool Editor::Gui::HotkeyManager::GetHotkeyPressed( uint32_t hotkey )
{
	auto& data = hotkeys[ hotkey ];
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
