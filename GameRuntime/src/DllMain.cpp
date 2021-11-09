#include <Windows.h>
#include <Pine/Pine.hpp>

#include "Pine/Entitylist/EntityList.hpp"
#include "Pine/RuntimeLoader/RuntimeLoader.hpp"

#include "Controllers/GameController/GameController.hpp"

extern "C" {

	bool __declspec( dllexport ) __stdcall ModuleInitialize( Pine::ModuleHandle* handle, const Pine::PineInstance* pineInstance ) {
		UseInstance( pineInstance );

		handle->RegisterNativeScript( "GameController", sizeof( GameController ), [ ] { return new GameController( ); } );

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

