#pragma once
#include "..\..\Components\Camera\Camera.hpp"
#include "..\..\OpenGL\FrameBuffer\FrameBuffer.hpp"

namespace Pine {

	struct RenderingContext {
		bool m_Is3D = false;

		int m_Width = 0;
		int m_Height = 0;

		Pine::Camera* m_Camera = nullptr;

		// If nullptr, the scene will just be fucking rendered to anything bound or the screen.
		Pine::FrameBuffer* m_FrameBuffer = nullptr;

	};

	RenderingContext* CreateDefaultRenderingContext( );

}