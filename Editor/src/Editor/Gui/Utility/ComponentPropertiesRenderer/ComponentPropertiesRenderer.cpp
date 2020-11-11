#include "ComponentPropertiesRenderer.hpp"
#include <Pine\Components\Transform\Transform.hpp>
#include "..\..\Widgets\Widgets.hpp"
#include <Pine\Components\ModelRenderer\ModelRenderer.hpp>

using namespace Editor::Gui;

namespace {

	void RenderTransform( Pine::Transform* transform ) {
		if ( !transform )
			return;
		
		Widgets::Vector3( "Position", transform->Position );
		Widgets::Vector3( "Rotation", transform->Rotation );
		Widgets::Vector3( "Scale", transform->Scale );
	}

	void RenderModelRenderer( Pine::ModelRenderer* renderer ) {
		if ( !renderer )
			return;

		if ( auto newAsset = Widgets::AssetPicker( "Model", renderer->GetTargetModel( ), true, Pine::EAssetType::Model ) ) {
			renderer->SetTargetModel( reinterpret_cast<Pine::Model*>( newAsset ) );
		}

		if ( auto newAsset = Widgets::AssetPicker( "Override model material", renderer->GetMaterialOverride( ), true, Pine::EAssetType::Material ) ) {
			renderer->SetMaterialOverride( reinterpret_cast< Pine::Material* >( newAsset ) );
		}
	}

}

void Editor::Gui::Utility::ComponentPropertiesRenderer::RenderComponentProperties( Pine::IComponent* component ) {
	
	switch ( component->GetType( ) ) {
	case Pine::EComponentType::Transform:
		RenderTransform( dynamic_cast< Pine::Transform* >( component ) ); break;
	case Pine::EComponentType::ModelRenderer:
		RenderModelRenderer( dynamic_cast< Pine::ModelRenderer* >( component ) ); break;
	default:
		break;
	}

}
