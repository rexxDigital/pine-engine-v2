#pragma once
#include "../../Core/Interfaces/Interfaces.hpp"

namespace Pine
{
	class Texture3D;

	class ISkybox : public IInterface
	{
	public:
		virtual void SetSkyboxCubemap( Texture3D* texture ) = 0;
		virtual Texture3D* GetSkyboxCubemap( ) = 0;

		virtual void Render( ) = 0;
	};

}