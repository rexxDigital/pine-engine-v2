#include "Script.hpp"

#include "../../ScriptManager/ScriptManager.hpp"

#include <angelscript.h>

// don't ask
#define ctx ScriptingManager::ScriptContext

Pine::Script::Script( )
{
	m_Type = EAssetType::Script;
}

asIScriptObject* Pine::Script::CreateObject( )
{
	if ( !m_ScriptClassInfo )
		return nullptr;
	
	const std::string className = m_FilePath.stem( ).string( );
	
	// Create instance of script class
	asIScriptFunction* factory = m_ScriptClassInfo->GetFactoryByDecl( std::string( className + " @" + className + "()" ).c_str( ) );

	ScriptingManager::ScriptContext->Prepare( factory );

	ScriptingManager::ScriptContext->Execute( );

	// Get script object instance
	asIScriptObject* obj = *reinterpret_cast< asIScriptObject** >( ScriptingManager::ScriptContext->GetAddressOfReturnValue( ) );

	// Make sure the AS(S) GC aint fuckin this bitch up
	obj->AddRef( );

	return obj;
}

void Pine::Script::CallOnSetup( asIScriptObject* thisPtr )
{
	if ( !m_fnOnSetup )
	{
		return;
	}

	ctx->Prepare( m_fnOnSetup );
	ctx->SetObject( thisPtr );
	ctx->Execute( );
}

void Pine::Script::OnCompile( )
{
	const std::string className = m_FilePath.stem( ).string( );

	// Find script class
	asITypeInfo* type = ScriptingManager::ScriptModule->GetTypeInfoByDecl( className.c_str( ) );
	if ( !type )
	{
		m_IsValid = false;
		return;
	}

	m_ScriptClassInfo = type;

	m_fnOnSetup = nullptr;
	m_fnOnUpdate = nullptr;

	// Find methods of script class
	m_fnOnSetup = type->GetMethodByDecl( "void OnSetup()" );
	m_fnOnUpdate = type->GetMethodByDecl( "void OnUpdate()" );

	m_IsValid = true;
}

void Pine::Script::AddReference( Behavior* ref )
{
	m_References.push_back( ref );
}

void Pine::Script::RemoveReference( Behavior* ref )
{
	for ( int i = 0; i < m_References.size( ); i++ )
	{
		if ( m_References[ i ] == ref )
		{
			m_References.erase( m_References.begin( ) + i );
			return;
		}
	}
}

bool Pine::Script::LoadFromFile( )
{
	return true;
}

bool Pine::Script::SaveToFile( )
{
	return true;
}

void Pine::Script::Dispose( )
{
}
