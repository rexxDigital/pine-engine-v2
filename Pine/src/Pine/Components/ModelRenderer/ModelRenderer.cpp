#include "ModelRenderer.hpp"

Pine::ModelRenderer::ModelRenderer() {
	m_ComponentType = EComponentType::ModelRenderer;
}

Pine::Model* Pine::ModelRenderer::GetTargetModel() const {
	return m_TargetModel;
}

void Pine::ModelRenderer::SetTargetModel(Pine::Model* mdl) {
	m_TargetModel = mdl;
}

void Pine::ModelRenderer::OnSetup() {
}

void Pine::ModelRenderer::OnUpdate(float deltaTime) {
}

void Pine::ModelRenderer::OnRender() {
}

Pine::IComponent* Pine::ModelRenderer::Clone( )
{
	return new Pine::ModelRenderer( );
}

