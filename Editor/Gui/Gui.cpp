#include "Gui.hpp"
#include "ImGui/imgui.h"

namespace {

	void SetupDockSpace() {
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->GetWorkPos());
		ImGui::SetNextWindowSize(viewport->GetWorkSize());
		ImGui::SetNextWindowViewport(viewport->ID);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

		window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::Begin("PineEditor", NULL, window_flags);

		ImGui::PopStyleVar(2);

		ImGuiIO& io = ImGui::GetIO();

		ImGuiID dockspace_id = ImGui::GetID("EngineDockSpace");
		ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	}

	void EndDockingSpace() {
		ImGui::End();
	}

}

void Editor::Gui::Run() {
	
	SetupDockSpace();

	ImGui::ShowDemoWindow(nullptr);

	if (ImGui::BeginMenuBar()) {
		if (ImGui::BeginMenu("Options")) {
			ImGui::EndMenu();
		}

		ImGui::EndMenuBar();
	}

	EndDockingSpace();


}
