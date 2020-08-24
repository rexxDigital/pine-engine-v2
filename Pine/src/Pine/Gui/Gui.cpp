#include "Gui.hpp"
#include "../../ImGui/imgui.h"
#include "../../ImGui/imgui_impl_glfw.h"
#include "../../ImGui/imgui_impl_opengl3.h"
#include "../Core/Window/Window.hpp"
#include <GLFW\glfw3.h>

void Pine::Gui::Setup() {
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsClassic();

	ImGui_ImplGlfw_InitForOpenGL(Window::Internal::GetWindowPointer(), true);
	ImGui_ImplOpenGL3_Init("#version 420");
}

void Pine::Gui::Dispose() {
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Pine::Gui::Render() {
	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::ShowDemoWindow(nullptr);

	int display_w, display_h;
	glfwGetFramebufferSize(Window::Internal::GetWindowPointer(), &display_w, &display_h);
	glViewport(0, 0, display_w, display_h);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
