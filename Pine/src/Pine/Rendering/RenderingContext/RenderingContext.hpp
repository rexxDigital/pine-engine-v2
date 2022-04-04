#pragma once
#include "..\..\Components\Camera\Camera.hpp"
#include "..\..\OpenGL\FrameBuffer\FrameBuffer.hpp"
#include <glm/vec4.hpp>

namespace Pine {

	struct RenderingContext
	{
		bool m_Is3D = false;
		bool m_AutoUpdateSize = true;
		
		int m_Width = 0;
		int m_Height = 0;

		// Statistics from previous frame
		int m_DrawCalls = 0;

        double m_TotalRenderTime = 0;

        double m_EntityUpdateTime = 0;
        std::array<double, 32> m_ComponentUpdateTime;

		double m_EntitySortTime = 0;
		double m_EntityRenderTime = 0;
		double m_PostProcessingTime = 0;
		
		Camera* m_Camera = nullptr;

		// If nullptr, the scene will just be rendered to any frame buffer bound or just the screen.
		FrameBuffer* m_FrameBuffer = nullptr;

		glm::vec4 m_ClearColor = glm::vec4( 0.f, 0.f, 0.f, 1.f );
	};

	RenderingContext* CreateDefaultRenderingContext( );

}