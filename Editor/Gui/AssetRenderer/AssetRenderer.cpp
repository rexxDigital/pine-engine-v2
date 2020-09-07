#include "AssetRenderer.hpp"



#include "../Widgets/Widgets.hpp"
#include "ImGui/imgui.h"
#include "Pine/Assets/Model/Model.hpp"
#include "Pine/Assets/Texture2D/Texture2D.hpp"

namespace
{

	void RenderTexture2D( Pine::Texture2D* texture )
	{
		ImGui::Text( "Width: %dpx", texture->GetWidth( ) );
		ImGui::Text( "Height: %dpx", texture->GetHeight( ) );
		ImGui::Text( "OpenGL ID: %d", texture->GetId( ) );
	}

	void RenderModel( Pine::Model* model )
	{

		for ( int i = 0; i < model->GetMeshList( ).size( ); i++ )
		{
			auto mesh = model->GetMeshList( )[ i ];

			if ( ImGui::CollapsingHeader( std::string( "Mesh #" + std::to_string( i ) + "##Model" + model->GetPath( ).string( ) ).c_str( ), ImGuiTreeNodeFlags_::ImGuiTreeNodeFlags_DefaultOpen ) )
			{

				ImGui::Text( "Render Vertices: %llu", mesh->GetRenderCount( ) );
				ImGui::Text( "Has Index Buffer: %d", mesh->HasElementBuffer( ) );

				if (const auto newMaterial = Gui::Widgets::AssetPicker( mesh->GetMaterial( ), "Mesh Material", Pine::EAssetType::Material ) )
				{
					mesh->SetMaterial( dynamic_cast< Pine::Material* >( newMaterial ) );
				}
			}

			ImGui::Spacing( );
		}

	}

	void RenderMaterial( Pine::Material* material )
	{
		if ( auto newDiffuse = Gui::Widgets::AssetPicker( material->GetDiffuse( ), "Diffuse Texture", Pine::EAssetType::Texture2D ) )
		{
			material->SetDiffuse( dynamic_cast< Pine::Texture2D* >( newDiffuse ) );
		}
		
		if ( auto newSpecular = Gui::Widgets::AssetPicker( material->GetSpecular( ), "Specular Map", Pine::EAssetType::Texture2D ) )
		{
			material->SetSpecular( dynamic_cast< Pine::Texture2D* >( newSpecular ) );
		}

		ImGui::Spacing( );

		ImGui::ColorEdit3( "Diffuse Color", reinterpret_cast< float* >( &material->DiffuseColor( ) ) );
		ImGui::ColorEdit3( "Specular Color", reinterpret_cast< float* >( &material->SpecularColor( ) ) );
		ImGui::ColorEdit3( "Ambient Color", reinterpret_cast< float* >( &material->AmbientColor( ) ) );

		float shininess = material->GetShininiess( );

		ImGui::Text( "Shininess Factor" );

		if ( ImGui::SliderFloat( "##Shininess", &shininess, 2.f, 128.f ) )
		{
			material->SetShininiess( shininess );
		}
	}

}

void Gui::AssetRenderer::Render( Pine::IAsset* asset )
{

	if ( asset->GetType( ) == Pine::EAssetType::Texture2D )
	{
		RenderTexture2D( dynamic_cast< Pine::Texture2D* >( asset ) );
	}

	if ( asset->GetType( ) == Pine::EAssetType::Model )
	{
		RenderModel( dynamic_cast< Pine::Model* >( asset ) );
	}

	if ( asset->GetType( ) == Pine::EAssetType::Material )
	{
		RenderMaterial( dynamic_cast< Pine::Material* >( asset ) );
	}

}
