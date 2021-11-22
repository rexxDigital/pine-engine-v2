#include "RenderingContext.hpp"
#include "..\..\Core\Window\Window.hpp"

Pine::RenderingContext* Pine::CreateDefaultRenderingContext( )
{
	const auto context = new RenderingContext;

	context->m_Is3D = true;

	context->m_Camera = nullptr;
	context->m_FrameBuffer = nullptr;

	context->m_Width = Window::GetSize( ).x;
	context->m_Height = Window::GetSize( ).y;

	return context;
}
