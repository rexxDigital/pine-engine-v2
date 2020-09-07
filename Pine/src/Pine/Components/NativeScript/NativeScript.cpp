#include "NativeScript.hpp"

Pine::NativeScript::NativeScript( )
{
	m_ComponentType = EComponentType::NativeScript;
}

Pine::IComponent* Pine::NativeScript::Clone( )
{
	return nullptr;
}
