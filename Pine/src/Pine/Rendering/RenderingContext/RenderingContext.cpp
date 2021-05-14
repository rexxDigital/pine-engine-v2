#include "RenderingContext.hpp"
#include "..\..\Core\Window\Window.hpp"

Pine::RenderingContext* Pine::CreateDefaultRenderingContext( ) {
	const auto context = new Pine::RenderingContext;

	context->m_Is3D = true;

	context->m_Camera = nullptr;
	context->m_FrameBuffer = nullptr;

	context->m_Width = Pine::Window::GetSize( ).x;
	context->m_Height = Pine::Window::GetSize( ).y;

	return context;
}
