#include "NativeScript.hpp"

Pine::NativeScript::NativeScript( )
{
	m_ComponentType = EComponentType::NativeScript;
}

void Pine::NativeScript::OnSetup( )
{
	if ( !m_CreateFromFactory )
		return;



}
