#include "NativeScript.hpp"
#include "../../RuntimeLoader/RuntimeLoader.hpp"
#include "../../Core/Log/Log.hpp"

void Pine::NativeScript::SetupInternalComponent( )
{
	m_InternalComponent->SetStandalone( true );
	m_InternalComponent->SetActive( GetActive( ) );
	m_InternalComponent->SetParent( GetParent( ) );

	m_InternalComponent->OnCreated( );
}

void Pine::NativeScript::CreateInternalComponent( )
{
	if ( !m_CreateFromFactory ) return;

	const auto factory = RuntimeLoader->FindNativeScriptFactory( m_FactoryName );

	if ( !factory ) return;

	m_InternalComponent = factory->m_Factory( );

	SetupInternalComponent( );
}

Pine::NativeScript::NativeScript( )
{
	m_ComponentType = ComponentType::NativeScript;
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
   strcpy(m_FactoryName, str.c_str());
}

const char* Pine::NativeScript::GetFactoryName( ) const
{
	return m_FactoryName;
}

void Pine::NativeScript::SetInternalComponent( IComponent* component )
{
	m_InternalComponent = component;

	SetupInternalComponent( );
}

Pine::IComponent* Pine::NativeScript::GetInternalComponent( ) const
{
	return m_InternalComponent;
}

void Pine::NativeScript::OnSetup( )
{
	if ( m_CreateFromFactory )
	{
		CreateInternalComponent( );
	}

	if ( !m_InternalComponent )
		return;

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
    SetFactoryName(j[ "factoryName" ]);
}

void Pine::NativeScript::SaveToJson( nlohmann::json& j )
{
	j[ "factoryName" ] = m_FactoryName;
}

void Pine::NativeScript::OnCreated( )
{
	if ( m_InternalComponent )
		m_InternalComponent->OnCreated( );
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

void Pine::NativeScript::OnRenderUI() {
    if ( m_InternalComponent )
        m_InternalComponent->OnRenderUI( );
}
