#include "Behavior.hpp"
#include "../../Assets/Script/Script.hpp"

#include <angelscript.h>

#include "../../Assets/Assets.hpp"

Pine::Behavior::Behavior( )
{
	m_ComponentType = Pine::EComponentType::Behavior;
}

void Pine::Behavior::SetAttachedScript( Script* sc )
{
	if ( m_AttachedScript != nullptr )
		m_AttachedScript->RemoveReference( this );
	if ( m_ScriptObject )
		m_ScriptObject->Release( );

	if ( !sc )
	{
		m_ScriptFile = "";
		return;
	}

	m_AttachedScript = sc;
	m_ScriptObject = sc->CreateObject( );

	sc->AddReference( this );

	m_ScriptFile = sc->GetPath( ).string( );
}

Pine::Script* Pine::Behavior::GetAttachedScript( ) const
{
	return m_AttachedScript;
}

void Pine::Behavior::SetScriptFile( const std::string& str )
{
	m_ScriptFile = str;
}

void Pine::Behavior::LoadScriptFile( )
{
	SetAttachedScript( Pine::Assets->GetAsset<Pine::Script>( m_ScriptFile ) );
}

void Pine::Behavior::OnSetup( )
{
	if ( m_AttachedScript )
		m_AttachedScript->CallOnSetup( m_ScriptObject );
}

void Pine::Behavior::OnUpdate( float deltaTime )
{
	if ( m_AttachedScript )
		m_AttachedScript->CallOnUpdate( m_ScriptObject, deltaTime );
}

void Pine::Behavior::OnRender( )
{
}

void Pine::Behavior::OnDestroyed( )
{
	if ( m_AttachedScript )
		m_AttachedScript->RemoveReference( this );
	if ( m_ScriptObject )
		m_ScriptObject->Release( );
}

void Pine::Behavior::SaveToJson( nlohmann::json& j )
{
	j[ "scriptFile" ] = m_ScriptFile;
}

void Pine::Behavior::LoadFromJson( nlohmann::json& j )
{
	if ( j.contains( "scriptFile" ) )
	{
		m_ScriptFile = j[ "scriptFile" ];
	}
}
