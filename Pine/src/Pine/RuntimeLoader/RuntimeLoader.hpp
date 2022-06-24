#pragma once
#include "../Core/Interfaces/Interfaces.hpp"

#include <string>
#include <filesystem>
#include <functional>

namespace Pine
{
	class NativeScript;

	struct NativeScriptFactory
	{
		std::string m_Name;
		size_t m_Size;
		std::function<NativeScript* ( )> m_Factory;
	};

	struct ModuleHandle
	{
		std::filesystem::path m_Path;

		bool m_Loaded = false;

		// The DLL module handle within WINAPI
		void* m_Instance = nullptr;

		std::vector<NativeScriptFactory> m_Factories;

		void RegisterNativeScript( const std::string& name, size_t size, std::function<NativeScript* ( )> factory );
	};

	typedef bool( *ModuleInitializeFn )( ModuleHandle* handle, PineInstance* pineInstance );

	class IRuntimeLoader : public IInterface
	{
	public:

		virtual ModuleHandle* LoadModule( const std::filesystem::path& modPath ) = 0;
		virtual bool UnloadModule( ModuleHandle* handle ) = 0;

		virtual NativeScriptFactory* FindNativeScriptFactory( const std::string& name ) = 0;

	};
}
