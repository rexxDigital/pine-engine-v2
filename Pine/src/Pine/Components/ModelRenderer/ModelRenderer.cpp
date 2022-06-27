#include "ModelRenderer.hpp"

#include "../../Core/Serialization/Serialization.hpp"

Pine::ModelRenderer::ModelRenderer( )
{
	m_ComponentType = ComponentType::ModelRenderer;
}

Pine::Model* Pine::ModelRenderer::GetModel( ) const
{
	return m_Model.Get( );
}

void Pine::ModelRenderer::SetModel( Model* mdl )
{
	m_Model = mdl;
}

Pine::Material* Pine::ModelRenderer::GetMaterialOverride( ) const
{
	return m_MaterialOverride.Get( );
}

void Pine::ModelRenderer::SetMaterialOverride( Material* mat )
{
	m_MaterialOverride = mat;
}

void Pine::ModelRenderer::OverrideStencilBuffer( bool enabled, std::uint8_t mask )
{
	m_OverrideStencilBuffer = enabled;
	m_StencilBufferMask = mask;
}

bool Pine::ModelRenderer::GetOverridingStencilBuffer( ) const
{
	return m_OverrideStencilBuffer;
}

std::uint8_t Pine::ModelRenderer::GetOverridedStencilBufferMask( ) const
{
	return m_StencilBufferMask;
}

void Pine::ModelRenderer::OnSetup( )
{
}

void Pine::ModelRenderer::OnUpdate( float deltaTime )
{
}

void Pine::ModelRenderer::OnRender( )
{
}

void Pine::ModelRenderer::SaveToJson( nlohmann::json& j )
{
	Serialization::SaveAsset( j[ "model" ], m_Model.Get( ) );
	Serialization::SaveAsset( j[ "matOverride" ], m_MaterialOverride.Get( ) );

    j["receiveShadows"] = m_ReceiveShadows;
}

void Pine::ModelRenderer::LoadFromJson( nlohmann::json& j )
{
	m_Model = dynamic_cast< Pine::Model* >( Serialization::LoadAsset( j, "model" ) );
	m_MaterialOverride = dynamic_cast< Pine::Material* >( Serialization::LoadAsset( j, "matOverride" ) );

    if (j.contains("receiveShadows"))
        m_ReceiveShadows = j["receiveShadows"].get<bool>();
}

bool Pine::ModelRenderer::GetReceiveShadows() const {
    return m_ReceiveShadows;
}

void Pine::ModelRenderer::SetReceiveShadows(bool value) {
    m_ReceiveShadows = value;
}
