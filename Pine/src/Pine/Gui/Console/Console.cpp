#include "Console.hpp"
#include "../../../ImGui/imgui.h"
#include <string>

namespace {
	
	std::string g_ConsoleContents = "";
	char buff[64];

}

void Pine::Gui::Console::Run()
{
	ImGui::SetNextWindowSize(ImVec2(700.f, 400.f));
	ImGui::Begin("Console", NULL, 0);
	{
		if (ImGui::InputTextMultiline("##ConsoleEditBox", g_ConsoleContents.data(), g_ConsoleContents.capacity(), ImVec2(-1.f, 340.f), ImGuiInputTextFlags_::ImGuiInputTextFlags_ReadOnly)) {
			
		}

		if (ImGui::InputText("##ConsoleCommand", buff, 64, 0)) {
			
		}
	}
	ImGui::End();
}
