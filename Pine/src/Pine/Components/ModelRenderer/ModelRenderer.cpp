#include "ModelRenderer.hpp"

#include "../../Core/Serialization/Serialization.hpp"

Pine::ModelRenderer::ModelRenderer( ) {
	m_ComponentType = EComponentType::ModelRenderer;
}

Pine::Model* Pine::ModelRenderer::GetTargetModel( ) const {
	return m_TargetModel;
}

void Pine::ModelRenderer::SetTargetModel( Pine::Model* mdl ) {
	m_TargetModel = mdl;
}

Pine::Material* Pine::ModelRenderer::GetMaterialOverride( ) const {
	return m_MaterialOverride;
}

void Pine::ModelRenderer::SetMaterialOverride( Pine::Material* mat ) {
	m_MaterialOverride = mat;
}

void Pine::ModelRenderer::OnSetup( ) {
}

void Pine::ModelRenderer::OnUpdate( float deltaTime ) {
}

void Pine::ModelRenderer::OnRender( ) {
}

void Pine::ModelRenderer::SaveToJson( nlohmann::json& j )
{
	Serialization::SaveAsset( j[ "model" ], m_TargetModel );
}

void Pine::ModelRenderer::LoadFromJson( nlohmann::json& j )
{
	m_TargetModel = dynamic_cast< Pine::Model* >( Serialization::LoadAsset( j, "model" ) );
}
