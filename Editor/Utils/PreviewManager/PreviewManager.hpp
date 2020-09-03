#pragma once
#include <glm/mat4x4.hpp>

#include "Pine/Assets/Mesh/Mesh.hpp"

namespace Pine { class FrameBuffer; }

namespace PreviewManager {

	void Setup();

	void PrepareRender(Pine::FrameBuffer* frameBuffer);
	void FinishRender();

	void RenderMesh( Pine::Mesh* mesh );
	
	void Dispose(); 

	void UseStaticTransformationMatrix( );
	void UseRotatingTransformationMatrix( );
	
	glm::mat4& GetRenderTransformationMatrix();

}