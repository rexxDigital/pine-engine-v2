#pragma once
#include "../IComponent/IComponent.hpp"
#include "../../Assets/Model/Model.hpp"

namespace Pine
{

	class ModelRenderer : public IComponent
	{
	private:
		AssetContainer<Model*> m_Model;
		AssetContainer<Material*> m_MaterialOverride;

		bool m_OverrideStencilBuffer = false;
		std::uint8_t m_StencilBufferMask = 0x00;

        bool m_ReceiveShadows = true;
    public:
		ModelRenderer( );

		Model* GetModel( ) const;
		void SetModel( Model* mdl );

		Material* GetMaterialOverride( ) const;
		void SetMaterialOverride( Material* mat );

		void OverrideStencilBuffer( bool enabled, std::uint8_t mask );

		bool GetOverridingStencilBuffer( ) const;
		std::uint8_t GetOverridedStencilBufferMask( ) const;

        bool GetReceiveShadows( ) const;
        void SetReceiveShadows( bool value );

		void OnSetup( ) override;
		void OnUpdate( float deltaTime ) override;
		void OnRender( ) override;

		void SaveToJson( nlohmann::json& j ) override;
		void LoadFromJson( nlohmann::json& j ) override;
	};

}