#pragma once
#include "../IComponent/IComponent.hpp"
#include "../../Assets/Model/Model.hpp"

namespace Pine
{

	class ModelRenderer : public IComponent
	{
	private:
		Pine::Model* m_TargetModel = nullptr;
		Pine::Material* m_MaterialOverride = nullptr;

		bool m_OverrideStencilBuffer = false;
		std::uint8_t m_StencilBufferMask = 0x00;
	public:
		ModelRenderer( );

		Pine::Model* GetTargetModel( ) const;
		void SetTargetModel( Pine::Model* mdl );

		Pine::Material* GetMaterialOverride( ) const;
		void SetMaterialOverride( Pine::Material* mat );

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