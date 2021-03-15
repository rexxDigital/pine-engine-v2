#pragma once
#include "../../Components/Behavior/Behavior.hpp"
#include "../IAsset/IAsset.hpp"

class asITypeInfo;
class asIScriptObject;
class asIScriptFunction;

namespace Pine
{

	class Script : public IAsset
	{
	private:
		asITypeInfo* m_ScriptClassInfo = nullptr;

		asIScriptFunction* m_fnOnSetup = nullptr;
		asIScriptFunction* m_fnOnUpdate = nullptr;
		
		bool m_IsValid = false;

		std::vector<Behavior*> m_References;

		std::string m_ScriptFileText = "";
	public:
		Script( );

		asIScriptObject* CreateObject( );

		const std::string& GetScriptFileText( ) const;

		bool HasOnSetup( ) const;
		bool HasOnUpdate( ) const;
		
		void CallOnSetup( asIScriptObject* thisPtr );
		void CallOnUpdate( asIScriptObject* thisPtr, float deltaTime );
		
		void OnCompile( );

		void AddReference( Behavior* ref );
		void RemoveReference( Behavior* ref );
		
		bool LoadFromFile( ) override;
		bool SaveToFile( ) override;
		void Dispose( ) override;
	};

	
}
