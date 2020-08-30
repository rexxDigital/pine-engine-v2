#include "Pine.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Rendering/RenderManager/RenderManager.hpp"
#include "UniformBuffers/UniformBuffers.hpp"
#include "Entitylist/EntityList.hpp"
#include "Gui/Gui.hpp"
#include "Rendering/Skybox/Skybox.hpp"
#include "Assets/Texture3D/Texture3D.hpp"
#include "Rendering/PreviewManager/PreviewManager.hpp"

namespace
{
	RenderCallback callback;

	Pine::FrameBuffer* g_TargetFrameBuffer = nullptr;
}

bool Pine::Setup()
{
	Log::Message("Setting up Pine...");

	if (!glfwInit())
	{
		Log::Fatal("Failed to initialize GLFW.");
		return false;
	}

	if (!Window::Internal::Create())
	{
		// We already print out an error message in the function, no need to do it again.
		return false;
	}

	glewExperimental = true;

	if (glewInit() != GLEW_OK)
	{
		Log::Fatal("Failed to initialize GLEW.");
		return false;
	}

	Log::Message("Loading engine shaders...");

	// We will have to setup the uniform buffers first.
	UniformBuffers::Setup();

	if (Assets::LoadFromDirectory("Engine\\Shaders") == 0) {
		Log::Fatal("Failed to load engine shaders.");
		return false;
	}

	Log::Message("Loading engine assets...");

	if (Assets::LoadFromDirectory("Engine") == 0) {
		Log::Fatal("Failed to load engine assets.");
		return false;
	}

	Renderer3D::Setup();
	Gui::Setup();
	Skybox::Setup();
	PreviewManager::Setup();

	Skybox::SetSkyboxCubemap(Assets::GetAsset<Pine::Texture3D>("Engine\\Skyboxes\\DefaultSkybox.cmap"));

	Log::Message("Pine was successfully initialized!");

	return true;
}

void Pine::Run()
{
	Window::Show();
	EntityList::RunOnSetup();

	Assets::RefreshDirectoryCache();
	Assets::GenerateAssetPreviews();

	const auto window = Window::Internal::GetWindowPointer();

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(window))
	{
		// Setup frame buffer
		if (g_TargetFrameBuffer != nullptr) {
			g_TargetFrameBuffer->Bind();
		}
		else {
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}

		glClearColor(0.f, 0.f, 0.f, 1.f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, g_TargetFrameBuffer->GetWidth(), g_TargetFrameBuffer->GetHeight());
		glEnable(GL_DEPTH_TEST);

		if (callback) {
			callback();
		}

		RenderManager::Run();

		Skybox::Render();

		// Reset the frame buffer for rendering.
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		Gui::Render();


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void Pine::Terminate()
{
	Assets::Dispose();
	UniformBuffers::Dispose();
	Renderer3D::Dispose();
	Gui::Dispose();
	Skybox::Dispose();
	PreviewManager::Dispose();

	Window::Internal::Destroy();
}

void Pine::SetRenderingCallback(RenderCallback fn)
{
	callback = fn;
}

void Pine::SetFrameBuffer(FrameBuffer* framebuffer)
{
	g_TargetFrameBuffer = framebuffer;
}

Pine::FrameBuffer* Pine::GetFrameBuffer()
{
	return g_TargetFrameBuffer;
}