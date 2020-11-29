#pragma once
#include "../IComponent/IComponent.hpp"

class asIScriptObject;

namespace Pine
{
	class Script;

	class Behavior : public IComponent
	{
	private:
		Script* m_AttachedScript = nullptr;
		asIScriptObject* m_ScriptObject = nullptr;

		std::string m_ScriptFile = "";
	public:
		Behavior( );

		void SetAttachedScript( Pine::Script* sc );
		Pine::Script* GetAttachedScript( ) const;

		void SetScriptFile( const std::string& str );
		void LoadScriptFile( );

		void OnSetup( ) override;
		void OnUpdate( float deltaTime ) override;
		void OnRender( ) override;
		void OnDestroy( ) override;
		IComponent* Clone( ) override;

		void SaveToJson( nlohmann::json& j ) override;
		void LoadFromJson( nlohmann::json& j ) override;

	};

}
