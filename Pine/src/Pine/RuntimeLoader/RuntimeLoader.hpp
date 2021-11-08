#pragma once
#include "../Core/Interfaces/Interfaces.hpp"

#include <string>
#include <filesystem>

namespace Pine
{
	struct ModuleHandle
	{
		bool m_Loaded = false;

		std::filesystem::path m_Path;

		void* m_Instance;
	};

	typedef bool( __stdcall* ModuleInitializeFn )( ModuleHandle* handle, PineInstance* pineInstance );

	class IRuntimeLoader : public IInterface
	{
	public:

		virtual ModuleHandle* LoadModule( const std::filesystem::path& modPath ) = 0;
		virtual bool UnloadModule( ModuleHandle* handle ) = 0;

	};
}
