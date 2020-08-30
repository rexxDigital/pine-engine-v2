#pragma once
#include <glm/mat4x4.hpp>

namespace Pine { class FrameBuffer; }

namespace Pine::PreviewManager {

	void Setup();

	void PrepareRender(FrameBuffer* frameBuffer);
	void FinishRender();

	void Dispose(); 

	glm::mat4& GetRenderTransformationMatrix();

}