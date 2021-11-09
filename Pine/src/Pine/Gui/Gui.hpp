#pragma once
#include "../Core/Interfaces/Interfaces.hpp"

typedef void ( *GuiRenderCallback )( );

namespace Pine
{

	class IGui : public IInterface
	{
	public:

		virtual void SetGuiRenderCallback( GuiRenderCallback callback ) = 0;
		virtual void Render( ) = 0;

	};

}