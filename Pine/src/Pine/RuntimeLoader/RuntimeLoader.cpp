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

			m_Modules.push_back( handle );

			return handle;
		}

		bool UnloadModule( ModuleHandle* handle ) override
		{
			const auto mod = static_cast< HMODULE >( handle->m_Instance );
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

			if ( loaded )
				FreeLibrary( mod );

			return true;
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
		f.m_Factory = factory;

		m_Factories.push_back( std::move( f ) );
	}

}
