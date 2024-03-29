#include <Pine/Pine.hpp>

#include "Pine/Entitylist/EntityList.hpp"
#include "Pine/RuntimeLoader/RuntimeLoader.hpp"

#include "Controllers/GameController/WorldController.hpp"
#include "Controllers/PlayerController/PlayerController.hpp"
#include "Controllers/MainMenuController/MainMenuController.hpp"
#include "Pine/Assets/Level/Level.hpp"

extern "C" {

	bool ModuleInitialize( Pine::ModuleHandle* handle, const Pine::PineInstance* pineInstance )
    {
		UseInstance( pineInstance );

		handle->RegisterNativeScript("WorldController", sizeof( WorldController ), [ ] { return new WorldController( ); } );
		handle->RegisterNativeScript("PlayerController", sizeof( PlayerController ), [ ] { return new PlayerController( ); } );
		handle->RegisterNativeScript("MainMenuController", sizeof( MainMenuController ), [ ] { return new MainMenuController( ); } );

		return true;
	}

}

#ifdef WIN32

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

#endif
