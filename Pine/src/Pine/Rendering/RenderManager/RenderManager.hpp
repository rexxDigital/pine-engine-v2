#pragma once

namespace Pine {
	class Camera;
}

namespace Pine::RenderingConfiguration
{

	inline int Width = 0;
	inline int Height = 0;
	
}

namespace Pine::RenderManager {

	void SetCamera(Camera* camera);
	Camera* GetCamera();
	
	void Run();

}