#include "Script.hpp"

#include "../../ScriptManager/ScriptManager.hpp"

#include <angelscript.h>
#include <fstream>

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

const std::string& Pine::Script::GetScriptFileText( ) const
{
	return m_ScriptFileText;
}

bool Pine::Script::HasOnSetup( ) const
{
	return m_fnOnSetup != nullptr;
}

bool Pine::Script::HasOnUpdate( ) const
{
	return m_fnOnUpdate != nullptr;
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

void Pine::Script::CallOnUpdate( asIScriptObject* thisPtr, float deltaTime )
{
	if ( !m_fnOnUpdate )
	{
		return;
	}

	ctx->Prepare( m_fnOnUpdate );
	ctx->SetObject( thisPtr );
	ctx->SetArgFloat( 0, deltaTime );
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
	m_fnOnUpdate = type->GetMethodByDecl( "void OnUpdate(float)" );

	// Read source code to string
	std::ifstream stream( m_FilePath );

	if ( stream.is_open(  ) )
	{
		std::string str( ( std::istreambuf_iterator<char>( stream ) ),
			std::istreambuf_iterator<char>( ) );

		m_ScriptFileText = str;
		
		stream.close( );
	}
	
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
