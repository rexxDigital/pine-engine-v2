#pragma once
#include "../IComponent/IComponent.hpp"
#include "../../Assets/Model/Model.hpp"

namespace Pine
{

	class ModelRenderer : public IComponent
	{
	private:
		Model* m_Model = nullptr;
		Material* m_MaterialOverride = nullptr;

		bool m_OverrideStencilBuffer = false;
		std::uint8_t m_StencilBufferMask = 0x00;
	public:
		ModelRenderer( );

		Model* GetModel( ) const;
		void SetModel( Model* mdl );

		Material* GetMaterialOverride( ) const;
		void SetMaterialOverride( Material* mat );

		void OverrideStencilBuffer( bool enabled, std::uint8_t mask );

		bool GetOverridingStencilBuffer( ) const;
		std::uint8_t GetOverridedStencilBufferMask( ) const;

		void OnSetup( ) override;
		void OnUpdate( float deltaTime ) override;
		void OnRender( ) override;

		void SaveToJson( nlohmann::json& j ) override;
		void LoadFromJson( nlohmann::json& j ) override;
	};

}