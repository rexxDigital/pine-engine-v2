#include "ScriptManager.hpp"

#include <angelscript.h>
#include <iostream>
#include <as_addons/scriptstdstring/scriptstdstring.h>
#include <as_addons/scriptbuilder/scriptbuilder.h>


#include "../Assets/Assets.hpp"
#include "../Assets/Script/Script.hpp"
#include "../Core/Log/Log.hpp"

using namespace Pine::ScriptingManager;

namespace
{

	CScriptBuilder* g_ScriptBuilder = nullptr;

	void Dispose( )
	{
		if ( ScriptContext )
			ScriptContext->Release( );
		if ( ScriptModule )
			ScriptModule->Discard( );

		delete g_ScriptBuilder;
	}

	void SetupContext( )
	{
		Dispose( );

		g_ScriptBuilder = new CScriptBuilder;

		const int result = g_ScriptBuilder->StartNewModule( ScriptEngine, "PineModule" );
		if ( result < 0 )
		{
			Pine::Log::Error( "Failed to create script module." );
		}

		ScriptModule = g_ScriptBuilder->GetModule( );
		ScriptContext = ScriptEngine->CreateContext( );
	}

	bool CompileScript( const std::string& path )
	{
		const int result = g_ScriptBuilder->AddSectionFromFile( path.c_str( ) );
		if ( result < 0 )
		{
			Pine::Log::Error( "Failed to load script file, " + path + " due to it containing errors." );
			return false;
		}

		return true;
	}

	void print( const std::string& str )
	{
		std::cout << str << std::endl;
	}
}

void Pine::ScriptingManager::CompileScripts( )
{
	Log::Message( "Compiling scripts..." );

	SetupContext( );

	// Keep a list of all scripts
	std::vector<Pine::Script*> compiledScripts;

	// off topic: thanks re sharper, this is cool.
	for ( const auto& [filePath, asset] : Pine::Assets::GetAssets( ) )
	{
		if ( asset->GetType( ) != EAssetType::Script )
			continue;

		const auto script = dynamic_cast< Pine::Script* >( asset );
		if ( !script )
			continue;

		if ( CompileScript( filePath ) )
		{
			compiledScripts.push_back( script );
		}
	}

	const int result = g_ScriptBuilder->BuildModule( );
	if ( 0 < result )
	{
		Log::Error( "Failed to compile script(s)." );
	}

	// Go through all scripts, tell em shit has compiled
	// and let em locate their shit.
	for ( auto script : compiledScripts )
	{
		script->OnCompile( );
	}

	Log::Message( "Compiled " + std::to_string( compiledScripts.size( ) ) + " script(s)." );
}

void Pine::ScriptingManager::Setup( )
{
	Log::Debug( "Pine::ScriptingManager::Setup()" );
	
	ScriptEngine = asCreateScriptEngine( );

	RegisterStdString( ScriptEngine );

	ScriptEngine->RegisterGlobalFunction( "void print(const string &in)", asFUNCTION( print ), asCALL_CDECL );
}

void Pine::ScriptingManager::Dispose( )
{
	ScriptEngine->ShutDownAndRelease( );
}
