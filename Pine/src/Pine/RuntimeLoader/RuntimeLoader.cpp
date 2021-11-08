#include "RuntimeLoader.hpp"

#include "../Pine.hpp"

#include <vector>
#include <Windows.h>

namespace Pine
{

	class CRuntimeLoader : public IRuntimeLoader
	{
	private:
		std::vector<ModuleHandle*> m_Modules;
	public:

		ModuleHandle* LoadModule( const std::filesystem::path& modPath ) override
		{
			Log->Message( "Attempting to load runtime library: " + modPath.filename( ).string( ) );

			if ( !std::filesystem::exists( modPath ) )
				return nullptr;
			if ( modPath.filename( ).extension( ).string( ) != ".dll" )
				return nullptr;

			const auto handle = new ModuleHandle;

			// Attempt to load the library into memory

			const auto dllHandle = LoadLibraryA( modPath.string( ).c_str( ) );

			if ( !dllHandle )
			{
				Log->Error( "Failed to load library, LoadLibraryA returned nullptr." );

				delete handle;

				return nullptr;
			}

			// Attempt to find and call the initialize module function every game runtime should have

			auto modInit = reinterpret_cast< ModuleInitializeFn >( GetProcAddress( dllHandle, "ModuleInitialize" ) );

			if ( !modInit )
			{
				Log->Error( "Failed to load library, could not find 'ModuleInitialize'" );

				FreeLibrary( dllHandle );

				delete handle;

				return nullptr;
			}

			modInit( handle, GetPineInstance( ) );

			handle->m_Loaded = true;
			handle->m_Path = modPath;
			handle->m_Instance = dllHandle;

			return handle;
		}

		bool UnloadModule( ModuleHandle* handle ) override
		{
			if ( handle->m_Loaded )
				FreeLibrary( static_cast< HMODULE >( handle->m_Instance ) );

			delete handle;

			return true;
		}

	};

	IRuntimeLoader* CreateRuntimeLoaderInterface( )
	{
		return new CRuntimeLoader;
	}

}