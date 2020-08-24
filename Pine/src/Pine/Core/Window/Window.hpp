#pragma once
#include <string>
#include <glm/ext/vector_int2.hpp>

struct GLFWwindow;

namespace Pine::Window::Internal
{
	bool Create();
	void Destroy();

	GLFWwindow* GetWindowPointer();
}

namespace Pine::Window
{
	void Show();
	void Hide();

	void SetPosition(int x, int y);
	void SetSize(int w, int h);
	void SetTitle(const std::string& str);

	glm::ivec2 GetSize();
}