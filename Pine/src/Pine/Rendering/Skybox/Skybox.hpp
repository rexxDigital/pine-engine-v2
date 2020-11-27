#pragma once

namespace Pine { class Texture3D; }

namespace Pine::Skybox {

	void Setup( );
	void Dispose( );

	void SetSkyboxCubemap( Texture3D* texture );
	Texture3D* GetSkyboxCubemap( );

	void Render( );

}