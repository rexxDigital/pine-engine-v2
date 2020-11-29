#pragma once

class asIScriptEngine;
class asIScriptModule;
class asIScriptContext;

namespace Pine::ScriptingManager
{
	inline asIScriptEngine* ScriptEngine = nullptr;
	inline asIScriptContext* ScriptContext = nullptr;
	inline asIScriptModule* ScriptModule = nullptr;
	
	void CompileScripts( );
	void Setup( );
	void Dispose( );
	
}
