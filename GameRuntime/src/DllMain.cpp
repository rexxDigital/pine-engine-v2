#include <Windows.h>
#include <Pine/Pine.hpp>

#include "Pine/Entitylist/EntityList.hpp"
#include "Pine/RuntimeLoader/RuntimeLoader.hpp"

#include "Controllers/GameController/GameController.hpp"
#include "Controllers/RGBExperiment/RGBExperiment.hpp"
#include "Controllers/InstancedRenderingTest/InstancedRenderingTest.hpp"
#include "Pine/Assets/Level/Level.hpp"

extern "C" {

	bool __declspec( dllexport ) __stdcall ModuleInitialize( Pine::ModuleHandle* handle, const Pine::PineInstance* pineInstance ) {
		UseInstance( pineInstance );

		handle->RegisterNativeScript( "GameController", sizeof( GameController ), [ ] { return new GameController( ); } );
		handle->RegisterNativeScript( "RGBExperiment", sizeof( RGBExperiment ), [ ] { return new RGBExperiment( ); } );
		handle->RegisterNativeScript( "InstancedRenderingTest", sizeof( InstancedRenderingTest ), [ ] { return new InstancedRenderingTest( ); } );

		return true;
	}

}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved )
{
	switch ( ul_reason_for_call )
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

