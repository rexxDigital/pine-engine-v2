#pragma once
#include "../../OpenGL/VertexArray/VertexArray.hpp"
#include "../../Assets/Mesh/Mesh.hpp"
#include "../../Assets/Terrain/Terrain.hpp"

namespace Pine
{
	class Camera;
	class Light;

	class IRenderer2D : public IInterface
	{
	public:

		virtual void PrepareSceneRendering( ) = 0;
		virtual void SetTargetFrameBuffer( Pine::FrameBuffer* buffer ) = 0;

		virtual void RenderFilledRect( int x, int y, int w, int h, glm::vec4 color ) = 0;
		virtual void RenderFilledRectTextured( int x, int y, int w, int h, Pine::Texture2D* texture ) = 0;

	};
}
