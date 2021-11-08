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
	ISkybox* CreateSkyboxInterface( );
	IGui* CreateGuiInterface( );
	IPostProcessing* CreatePostProcessingInterface( );
	IScriptManager* CreateScriptingManagerInterface( );
	IRuntimeLoader* CreateRuntimeLoaderInterface( );
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
	in.Skybox = CreateSkyboxInterface( );
	in.Gui = CreateGuiInterface( );
	in.PostProcessing = CreatePostProcessingInterface( );
	in.ScriptingManager = CreateScriptingManagerInterface( );
	in.RuntimeLoader = CreateRuntimeLoaderInterface( );

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
	Skybox = instance->Skybox;
	Gui = instance->Gui;
	PostProcessing = instance->PostProcessing;
	ScriptingManager = instance->ScriptingManager;
	RuntimeLoader = instance->RuntimeLoader;
}
