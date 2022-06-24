#include "RuntimeLoader.hpp"

#include "../Pine.hpp"

#include <utility>
#include <vector>

#ifdef _WIN32
    #include <Windows.h>
#else
    #include <dlfcn.h>
#endif

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

    #ifdef WIN32

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

			const auto modInit = reinterpret_cast< ModuleInitializeFn >( GetProcAddress( dllHandle, "ModuleInitialize" ) );

			if ( !modInit )
			{
				Log->Error( "Failed to load library, could not find 'ModuleInitialize'" );

				FreeLibrary( dllHandle );

				delete handle;

				return nullptr;
			}

    #else

            if ( modPath.filename( ).extension( ).string( ) != ".so" )
                return nullptr;

            const auto handle = new ModuleHandle;

            // Attempt to load the library into memory



            const auto dllHandle = dlopen( std::filesystem::absolute(modPath).string( ).c_str( ), RTLD_NOW );

            if ( !dllHandle )
            {
                Log->Error( "Failed to load library, dlopen returned nullptr, reason: " + std::string( dlerror( ) ) );


                return nullptr;
            }

            const auto modInit = reinterpret_cast< ModuleInitializeFn >( dlsym( dllHandle, "ModuleInitialize" ) );

            if ( !modInit )
            {
                Log->Error( "Failed to load library, could not find 'ModuleInitialize'" );

                dlclose( dllHandle );

                return nullptr;
            }

    #endif

            modInit( handle, GetPineInstance( ) );

            handle->m_Loaded = true;
            handle->m_Path = modPath;
            handle->m_Instance = dllHandle;

            m_Modules.push_back( handle );

            return handle;

		}

		bool UnloadModule( ModuleHandle* handle ) override
		{
            const auto mod = handle->m_Instance;
            const auto loaded = handle->m_Loaded;

            for ( int i = 0; i < m_Modules.size( ); i++ )
            {
                if ( handle == m_Modules[ i ] )
                {
                    m_Modules.erase( m_Modules.begin( ) + i );
                    break;
                }
            }

            delete handle;

			#ifdef WIN32

			if ( loaded )
				FreeLibrary( static_cast< HMODULE >( mod ) );

			return true;

			#else

            if (loaded)
                dlclose(mod);

            return true;

			#endif
		}

		NativeScriptFactory* FindNativeScriptFactory( const std::string& name ) override
		{
			for ( const auto mod : m_Modules )
			{
				if ( !mod->m_Loaded ) continue;

				for ( auto& factory : mod->m_Factories )
				{
					if ( factory.m_Name == name )
					{
						return &factory;
					}
				}
			}

			return nullptr;
		}

	};

	IRuntimeLoader* CreateRuntimeLoaderInterface( )
	{
		return new CRuntimeLoader;
	}

	void ModuleHandle::RegisterNativeScript( const std::string& name, size_t size, std::function<NativeScript* ( )> factory )
	{
		NativeScriptFactory f;

		f.m_Name = name;
		f.m_Size = size;
		f.m_Factory = std::move( factory );

		m_Factories.push_back( std::move( f ) );
	}

}
