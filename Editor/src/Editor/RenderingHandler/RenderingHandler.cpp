#include "RenderingHandler.hpp"
#include <Pine/Pine.hpp>
#include <Pine/Rendering/RenderManager/RenderManager.hpp>
#include <Pine/OpenGL/FrameBuffer/FrameBuffer.hpp>

namespace
{
	Pine::FrameBuffer* g_RenderingFrameBuffer = nullptr;
}

void Editor::RenderingHandler::Setup()
{
	g_RenderingFrameBuffer = new Pine::FrameBuffer();
	g_RenderingFrameBuffer->Create(1600, 900);

	Pine::RenderManager::GetRenderingContext()->m_FrameBuffer = g_RenderingFrameBuffer;
	
	Pine::RenderManager::GetRenderingContext()->m_Width = 1920;
	Pine::RenderManager::GetRenderingContext()->m_Height = 1080;
	
	Pine::RenderManager::GetRenderingContext()->m_AutoUpdateSize = false;
	
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
