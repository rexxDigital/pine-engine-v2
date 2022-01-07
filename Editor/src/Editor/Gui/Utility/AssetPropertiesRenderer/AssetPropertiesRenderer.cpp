#include "AssetPropertiesRenderer.hpp"
#include <Pine\Assets\IAsset\IAsset.hpp>
#include <Pine\Assets\Texture2D\Texture2D.hpp>
#include <ImGui\imgui.h>
#include <Pine\Assets\Material\Material.hpp>

#include "../../../ProjectManager/ProjectManager.hpp"
#include "..\..\Widgets\Widgets.hpp"
#include "Editor/Gui/Gui.hpp"
#include "Pine/Assets/Level/Level.hpp"
#include "Pine/Assets/Model/Model.hpp"
#include "Pine/Assets/Texture3D/Texture3D.hpp"
#include "Pine/Rendering/Skybox/Skybox.hpp"

void UpdateAssetCache( );

namespace
{

	void RenderTexture2D( Pine::Texture2D* texture2d )
	{
		constexpr float padding = 256.f;

		const auto availableArea = ImGui::GetContentRegionAvail( );

		if ( 0.f > availableArea.x )
		{
			return;
		}

		ImGui::Image( reinterpret_cast< ImTextureID >( texture2d->GetId( ) ), ImVec2( availableArea.x - padding, availableArea.x - padding ) );

		ImGui::Spacing( );
		ImGui::Separator( );
		ImGui::Spacing( );

		ImGui::BeginChild( "Texture2DProperties", ImVec2( -1.f, 90.f ), false );

		ImGui::Columns( 2, nullptr, false );

		ImGui::Text( "OpenGL ID" );
		ImGui::Text( "Width" );
		ImGui::Text( "Height" );
		ImGui::Text( "Channels" );

		ImGui::NextColumn( );

		ImGui::Text( "%d", texture2d->GetId( ) );
		ImGui::Text( "%d", texture2d->GetWidth( ) );
		ImGui::Text( "%d", texture2d->GetHeight( ) );

		ImGui::Text( "%s", texture2d->GetChannels( ) == 3 ? "RGB" : "RGBA" ); // we don't do any other channels ok

		ImGui::Columns( 1 );

		ImGui::EndChild( );
	}

	void RenderMaterial( Pine::Material* mat )
	{
		const auto diffuseAssetRet = Editor::Gui::Widgets::AssetPicker( "Diffuse", mat->GetDiffuse( ), true, Pine::AssetType::Texture2D );
		if ( diffuseAssetRet.valid )
		{
			mat->SetDiffuse( dynamic_cast< Pine::Texture2D* >( diffuseAssetRet.asset ) );
		}

		const auto specularAssetRet = Editor::Gui::Widgets::AssetPicker( "Specular Map", mat->GetSpecular( ), true, Pine::AssetType::Texture2D );
		if ( specularAssetRet.valid )
		{
			mat->SetSpecular( dynamic_cast< Pine::Texture2D* >( specularAssetRet.asset ) );
		}

		auto normalMapAssetRet = Editor::Gui::Widgets::AssetPicker( "Normal Map", mat->GetNormal( ), true, Pine::AssetType::Texture2D );
		if ( specularAssetRet.valid )
		{
			mat->SetSpecular( dynamic_cast< Pine::Texture2D* >( specularAssetRet.asset ) );
		}

		if ( Editor::Gui::Widgets::ColorPicker( "Diffuse Color", mat->DiffuseColor( ) ) )
			mat->SetUpdated( true );

		if ( Editor::Gui::Widgets::ColorPicker( "Specular Color", mat->SpecularColor( ) ) )
			mat->SetUpdated( true );

		if ( Editor::Gui::Widgets::ColorPicker( "Ambient Color", mat->AmbientColor( ) ) )
			mat->SetUpdated( true );

		int renderingMode = static_cast< int >( mat->GetRenderingMode( ) );
		if ( Editor::Gui::Widgets::Combobox( "Rendering Mode", renderingMode, "Opaque\0Discard\0Transparent\0" ) )
		{
			mat->SetRenderingMode( static_cast< Pine::MatRenderingMode >( renderingMode ) );
		}

		ImGui::Columns( 2, nullptr, false );
		ImGui::Text( "Shininess" );
		ImGui::NextColumn( );

		ImGui::SetNextItemWidth( -1.f );

		float sh = mat->GetShininess( );
		if ( ImGui::SliderFloat( "##Shininiess", &sh, 1.f, 128.f, "%.1f", 1.f ) )
			mat->SetShininess( sh );

		ImGui::Columns( 1 );

		float txScale = mat->GetTextureScale( );
		if ( Editor::Gui::Widgets::SliderFloat( "Texture Scale", txScale, 1.f, 16.f, "%.0f" ) )
		{
			mat->SetTextureScale( txScale );
		}

		const auto shaderRet = Editor::Gui::Widgets::AssetPicker( "Shader", mat->GetShader( ), true, Pine::AssetType::Shader );
		if ( shaderRet.valid )
		{
			mat->SetShader( dynamic_cast< Pine::Shader* >( shaderRet.asset ) );
		}

		auto renderFlags = mat->GetRenderFlags( );

		bool disableBackfaceCulling = renderFlags & Pine::RenderFlags::DisableBackfaceCulling;
		if ( Editor::Gui::Widgets::Checkbox( "Disable backface culling", disableBackfaceCulling ) )
		{
			if ( disableBackfaceCulling )
				renderFlags |= Pine::RenderFlags::DisableBackfaceCulling;
			else
				renderFlags &= ~Pine::RenderFlags::DisableBackfaceCulling;

			mat->SetRenderFlags( renderFlags );
		}

		bool wireframeRendering = renderFlags & Pine::RenderFlags::RenderWireframe;
		if ( Editor::Gui::Widgets::Checkbox( "Render Wireframe", wireframeRendering ) )
		{
			if ( wireframeRendering )
				renderFlags |= Pine::RenderFlags::RenderWireframe;
			else
				renderFlags &= ~Pine::RenderFlags::RenderWireframe;

			mat->SetRenderFlags( renderFlags );
		}

		auto shaderProperties = mat->GetShaderProperties( );

		bool shaderPerformanceMode = shaderProperties & Pine::ShaderProperties::PerformanceFast;
		if ( Editor::Gui::Widgets::Checkbox( "Shader Performance Mode", shaderPerformanceMode ) )
		{
			if ( shaderPerformanceMode ) 
				shaderProperties |= Pine::ShaderProperties::PerformanceFast;
			else 
				shaderProperties &= ~Pine::ShaderProperties::PerformanceFast;

			mat->SetShaderProperties( shaderProperties );
		}

		if ( ImGui::IsItemHovered( ) )
		{
			ImGui::SetTooltip( "Requests an optimized version of the shader, may not always be avaliable." );
		}



	}

	void RenderModel( const Pine::Model* model )
	{
		for ( int i = 0; i < model->GetMeshList( ).size( ); i++ )
		{
			if ( ImGui::CollapsingHeader( std::string( "Mesh #" + std::to_string( i ) ).c_str( ), ImGuiTreeNodeFlags_DefaultOpen ) )
			{
				const auto mesh = model->GetMeshList( )[ i ];

				ImGui::Columns( 2, nullptr, false );

				ImGui::Text( "Render Count" );
				ImGui::Text( "Has Element Buffer" );

				ImGui::NextColumn( );

				ImGui::Text( "%d", mesh->GetRenderCount( ) );
				ImGui::Text( "%d", mesh->HasElementBuffer( ) );

				ImGui::Columns( 1 );

				const auto materialRet = Editor::Gui::Widgets::AssetPicker( "Material", mesh->GetMaterial( ), true, Pine::AssetType::Material );
				if ( materialRet.valid )
				{
					mesh->SetMaterial( dynamic_cast< Pine::Material* >( materialRet.asset ) );
				}

				const auto material = mesh->GetMaterial( );

				bool showButton = false;

				if ( material && material->GetReadOnly( ) )
					showButton = true;
				if ( material && material->IsGenerated( ) )
					showButton = true;

				if ( showButton && ImGui::Button( std::string( "Create Custom Material##" + std::to_string( i ) ).c_str( ) ) )
				{
					// Clean readable code :tm:
					std::string materialFile = model->GetPath( ).parent_path( ).string( ) + "\\" + model->GetPath( ).stem( ).string( ) + ".mat";

					if ( std::filesystem::exists( materialFile ) )
					{
						mesh->SetMaterial( dynamic_cast< Pine::Material* >( Pine::Assets->LoadFromFile( materialFile ) ) );
						return;
					}

					std::filesystem::copy( "Assets\\Engine\\Materials\\Default.mat", materialFile );

					mesh->SetMaterial( dynamic_cast< Pine::Material* >( Pine::Assets->LoadFromFile( materialFile ) ) );

					UpdateAssetCache( );
				}
			}
		}
	}

	void RenderLevel( Pine::Level* lvl )
	{
		if ( !lvl )
			return;

		ImGui::Text( "Blueprints: %d", lvl->GetBlueprintCount( ) );

		if ( ImGui::Button( "Load", ImVec2( 300.f, 40.f ) ) )
		{
			lvl->Load( );
			Editor::ProjectManager::OpenLevel( lvl );
		}

		if ( ImGui::Button( "Save", ImVec2( 300.f, 40.f ) ) )
		{
			lvl->CreateFromCurrentLevel( );
			Editor::ProjectManager::OpenLevel( lvl );
		}
	}

	void RenderCubemap( Pine::Texture3D* cubeMap )
	{
		if ( !cubeMap )
			return;

		// Hack to allow the button even it's read only.
		if ( cubeMap->GetReadOnly( ) )
			Editor::Gui::Widgets::PopDisabled( );

		if ( ImGui::Button( "Use as sky box" ) )
		{
			Pine::Skybox->SetSkyboxCubemap( cubeMap );
		}

		if ( cubeMap->GetReadOnly( ) )
			Editor::Gui::Widgets::PushDisabled( );
	}

}

void Editor::Gui::Utility::AssetPropertiesRenderer::RenderAssetProperties( Pine::IAsset* asset )
{
	if ( !asset )
		return;

	if ( asset->GetReadOnly(  ) )
	{
		Widgets::PushDisabled( );
	}
 
	switch ( asset->GetType( ) )
	{
	case Pine::AssetType::Texture2D:
		RenderTexture2D( dynamic_cast< Pine::Texture2D* >( asset ) );
		break;
	case Pine::AssetType::Material:
		RenderMaterial( dynamic_cast< Pine::Material* >( asset ) );
		break;
	case Pine::AssetType::Model:
		RenderModel( dynamic_cast< Pine::Model* >( asset ) );
		break;
	case Pine::AssetType::Level:
		RenderLevel( dynamic_cast< Pine::Level* >( asset ) );
		break;
	case Pine::AssetType::Texture3D:
		RenderCubemap( dynamic_cast< Pine::Texture3D* >( asset ) );
		break;
	default:
		break;
	}

	if ( asset->GetReadOnly( ) )
	{
		Widgets::PopDisabled( );
	}
}
