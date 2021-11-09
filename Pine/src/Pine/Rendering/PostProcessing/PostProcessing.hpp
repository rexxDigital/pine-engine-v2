#pragma once
#include "../../Core/Interfaces/Interfaces.hpp"

namespace Pine
{
	class FrameBuffer;

	class IPostProcessing : public IInterface
	{
	public:

		virtual void Render( ) = 0;
		virtual FrameBuffer* GetRenderBuffer( ) = 0;

	};


}
