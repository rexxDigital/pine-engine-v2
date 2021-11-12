#include "NativeScript.hpp"
#include "../../RuntimeLoader/RuntimeLoader.hpp"

void Pine::NativeScript::CreateInternalComponent( )
{
	if ( !m_CreateFromFactory ) return;

	const auto factory = RuntimeLoader->FindNativeScriptFactory( m_FactoryName );

	if ( !factory ) return;

	m_InternalComponent = factory->m_Factory( );

	m_InternalComponent->SetStandalone( true );
	m_InternalComponent->SetActive( GetActive( ) );
	m_InternalComponent->SetParent( GetParent( ) );

	m_InternalComponent->OnCreated( );
}

Pine::NativeScript::NativeScript( )
{
	m_ComponentType = EComponentType::NativeScript;
}

void Pine::NativeScript::SetCreateFromFactory( bool value )
{
	m_CreateFromFactory = value;
}

bool Pine::NativeScript::GetCreateFromFactory( ) const
{
	return m_CreateFromFactory;
}

void Pine::NativeScript::SetFactoryName( const std::string& str )
{
	m_FactoryName = str;
}

const std::string& Pine::NativeScript::GetFactoryName( ) const
{
	return m_FactoryName;
}

void Pine::NativeScript::OnSetup( )
{
	if ( !m_CreateFromFactory )
		return;

	CreateInternalComponent( );

	m_InternalComponent->OnSetup( );
}

void Pine::NativeScript::OnRender( )
{
	if ( m_InternalComponent )
		m_InternalComponent->OnRender( );
}

void Pine::NativeScript::OnUpdate( float deltaTime )
{
	if ( m_InternalComponent )
		m_InternalComponent->OnUpdate( deltaTime );
}

void Pine::NativeScript::LoadFromJson( nlohmann::json& j )
{
	m_FactoryName = j[ "factoryName" ];
}

void Pine::NativeScript::SaveToJson( nlohmann::json& j )
{
	j[ "factoryName" ] = m_FactoryName;
}

void Pine::NativeScript::OnDestroyed( )
{
	if ( m_InternalComponent )
	{
		m_InternalComponent->OnDestroyed( );

		delete m_InternalComponent;

		m_InternalComponent = nullptr;
	}
}
