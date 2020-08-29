#pragma once

namespace Pine {
	class Camera;
}

namespace Pine::RenderManager {

	void SetCamera(Camera* camera);
	Camera* GetCamera();
	
	void Run();

}