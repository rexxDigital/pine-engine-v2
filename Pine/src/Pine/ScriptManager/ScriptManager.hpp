#pragma once
#include "../Core/Interfaces/Interfaces.hpp"

class asIScriptEngine;
class asIScriptModule;
class asIScriptContext;

namespace Pine
{

	class IScriptManager : public IInterface
	{
	public:
		virtual void CompileScripts( ) = 0;

		asIScriptEngine* ScriptEngine = nullptr;
		asIScriptContext* ScriptContext = nullptr;
		asIScriptModule* ScriptModule = nullptr;
	};

}
