#include "Interfaces.hpp"

namespace Pine
{
	ILog* CreateLogInterface( );
	IAssets* CreateAssetsInterface( );
	IComponents* CreateComponentsInterface( );
	IEntityList* CreateEntityListInterface( );
	IInputSystem* CreateInputSystemInterface( );
	IPhysicsManager* CreatePhysicsManagerInterface( );
	IRenderManager* CreateRenderManagerInterface( );
	IRenderer3D* CreateRenderer3DInterface( );
	IRenderer2D* CreateRenderer2DInterface( );
	ISkybox* CreateSkyboxInterface( );
	IGui* CreateGuiInterface( );
	IPostProcessing* CreatePostProcessingInterface( );
	IRuntimeLoader* CreateRuntimeLoaderInterface( );
	IDebugOverlay* CreateDebugOverlay( );
	IMath* CreateMath( );
}

void Pine::IInterface::Setup( )
{
}

void Pine::IInterface::Dispose( )
{
}

Pine::PineInstance Pine::CreateInstance( )
{
	PineInstance in;

	in.Log = CreateLogInterface( );
	in.Assets = CreateAssetsInterface( );
	in.Components = CreateComponentsInterface( );
	in.EntityList = CreateEntityListInterface( );
	in.Input = CreateInputSystemInterface( );
	in.PhysicsManager = CreatePhysicsManagerInterface( );
	in.RenderManager = CreateRenderManagerInterface( );
	in.Renderer3D = CreateRenderer3DInterface( );
	in.Renderer2D = CreateRenderer2DInterface( );
	in.Skybox = CreateSkyboxInterface( );
	in.Gui = CreateGuiInterface( );
	in.PostProcessing = CreatePostProcessingInterface( );
	in.RuntimeLoader = CreateRuntimeLoaderInterface( );
	in.DebugOverlay = CreateDebugOverlay( );
	in.Math = CreateMath( );

	return in;
}

void Pine::UseInstance( const PineInstance* instance )
{
	Log = instance->Log;
	Assets = instance->Assets;
	Components = instance->Components;
	EntityList = instance->EntityList;
	Input = instance->Input;
	PhysicsManager = instance->PhysicsManager;
	RenderManager = instance->RenderManager;
	Renderer3D = instance->Renderer3D;
	Renderer2D = instance->Renderer2D;
	Skybox = instance->Skybox;
	Gui = instance->Gui;
	PostProcessing = instance->PostProcessing;
	RuntimeLoader = instance->RuntimeLoader;
	DebugOverlay = instance->DebugOverlay;
	Math = instance->Math;
}
