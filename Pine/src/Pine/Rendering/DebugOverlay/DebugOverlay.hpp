#pragma once
#include <string>
#include <glm/vec4.hpp>

#include "../../Core/Interfaces/Interfaces.hpp"

namespace Pine
{

	class IDebugOverlay : public IInterface
	{
	public:

		virtual void SetViewport( int x, int y, int w, int h ) = 0;
		virtual glm::vec4 GetViewport( ) = 0;

		virtual void Render( ) = 0;

		virtual void RenderRectangle( int x, int y, int w, int h, glm::vec4 color ) = 0;
		virtual void RenderFilledRectangle( int x, int y, int w, int h, glm::vec4 color ) = 0;
		virtual void RenderText( int x, int y, const std::string& text, glm::vec4 color ) = 0;



	};


}
