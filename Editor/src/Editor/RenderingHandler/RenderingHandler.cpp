#include "RenderingHandler.hpp"
#include <Pine/Pine.hpp>
#include <Pine/Rendering/RenderManager/RenderManager.hpp>
#include <Pine/OpenGL/FrameBuffer/FrameBuffer.hpp>

#include <ImGui/imgui.h>


#include "Editor/EditorEntity/EditorEntity.hpp"
#include "Editor/Gui/Gui.hpp"

namespace
{
	Pine::FrameBuffer* g_RenderingFrameBuffer = nullptr;

	Pine::Camera* g_BackupCamera = nullptr;
	Pine::Camera* g_EditorCamera = nullptr;
	
	void OnPreRender()
	{
		// Render with level editor entity camera
		if (Editor::Gui::Globals::IsInLevelView)
		{
			g_BackupCamera = Pine::RenderManager::GetRenderingContext()->m_Camera;
			Pine::RenderManager::GetRenderingContext()->m_Camera = g_EditorCamera;
		}
	}

	void OnPostRender()
	{ 
		if (Editor::Gui::Globals::IsInLevelView)
		{
			Pine::RenderManager::GetRenderingContext()->m_Camera = g_BackupCamera;
		}
	}
}

void Editor::RenderingHandler::Setup()
{
	g_RenderingFrameBuffer = new Pine::FrameBuffer();
	g_RenderingFrameBuffer->Create(1600, 900);

	Pine::RenderManager::GetRenderingContext()->m_FrameBuffer = g_RenderingFrameBuffer;
	
	Pine::RenderManager::GetRenderingContext()->m_Width = 1920;
	Pine::RenderManager::GetRenderingContext()->m_Height = 1080;
	
	Pine::RenderManager::GetRenderingContext()->m_AutoUpdateSize = false;

	Pine::RenderManager::SetPreRenderingCallback(OnPreRender);
	Pine::RenderManager::SetPostRenderingCallback(OnPostRender);

	g_EditorCamera = Editor::EditorEntity::GetEntity()->GetComponent<Pine::Camera>();
}

Pine::FrameBuffer* Editor::RenderingHandler::GetFrameBuffer()
{
	return g_RenderingFrameBuffer;
}

void Editor::RenderingHandler::SetViewportSize(int w, int h)
{
	Pine::RenderManager::GetRenderingContext()->m_Width = w;
	Pine::RenderManager::GetRenderingContext()->m_Height = h;
}
