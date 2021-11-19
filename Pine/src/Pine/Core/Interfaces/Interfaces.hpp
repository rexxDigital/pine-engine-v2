#pragma once

namespace Pine
{

	class IInterface
	{
	public:
		virtual ~IInterface( ) = default;

		virtual void Setup( );
		virtual void Dispose( );
	};

	class ILog;
	class IAssets;
	class IComponents;
	class IWindowManager;
	class IEntityList;
	class IInputSystem;
	class IPhysicsManager;
	class IRenderManager;
	class IRenderer3D;
	class ISkybox;
	class IGui;
	class IPostProcessing;
	class IRuntimeLoader;

	inline ILog* Log;
	inline IAssets* Assets;
	inline IComponents* Components;
	inline IWindowManager* WindowManager;
	inline IEntityList* EntityList;
	inline IInputSystem* Input;
	inline IPhysicsManager* PhysicsManager;
	inline IRenderManager* RenderManager;
	inline IRenderer3D* Renderer3D;
	inline ISkybox* Skybox;
	inline IGui* Gui;
	inline IPostProcessing* PostProcessing;
	inline IRuntimeLoader* RuntimeLoader;

	struct PineInstance
	{
		ILog* Log;
		IAssets* Assets;
		IComponents* Components;
		IWindowManager* WindowManager;
		IEntityList* EntityList;
		IInputSystem* Input;
		IPhysicsManager* PhysicsManager;
		IRenderManager* RenderManager;
		IRenderer3D* Renderer3D;
		ISkybox* Skybox;
		IGui* Gui;
		IPostProcessing* PostProcessing;
		IRuntimeLoader* RuntimeLoader;
	};

	PineInstance CreateInstance( );

	void UseInstance(const PineInstance* instance);

}
