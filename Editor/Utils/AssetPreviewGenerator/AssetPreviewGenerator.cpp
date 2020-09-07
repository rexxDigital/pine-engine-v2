#include "AssetPreviewGenerator.hpp"
#include <Pine\Assets\Assets.hpp>
#include <unordered_map>



#include "../PreviewManager/PreviewManager.hpp"
#include "Pine/Assets/Model/Model.hpp"
#include "Pine/Assets/Texture2D/Texture2D.hpp"
#include "Pine/Core/Window/Window.hpp"
#include "Pine/OpenGL/FrameBuffer/FrameBuffer.hpp"
#include "Pine/Rendering/RenderManager/RenderManager.hpp"

namespace
{
	std::unordered_map<Pine::IAsset*, Editor::AssetPreviewGenerator::AssetPreview_t*> m_AssetPreviews;

	Pine::FrameBuffer* g_FullscreenFrameBuffer = nullptr;
	
	void GenerateTexturePreview( Pine::IAsset* asset, Editor::AssetPreviewGenerator::AssetPreview_t* preview )
	{
		const auto texture = dynamic_cast< Pine::Texture2D* >( asset );

		preview->invertUVs = false;
		preview->texture = texture->GetId( );
	}

	void GenerateModelPreview( Pine::IAsset* asset, Editor::AssetPreviewGenerator::AssetPreview_t* preview, Pine::FrameBuffer* buffer, bool rotating )
	{
		const auto model = dynamic_cast< Pine::Model* >( asset );

		if ( !buffer )
		{
			preview->frameBuffer = new Pine::FrameBuffer( );
			preview->frameBuffer->Create( 64, 64 );

			buffer = preview->frameBuffer;
		}

		Pine::RenderingConfiguration::Width = buffer->GetWidth(  );
		Pine::RenderingConfiguration::Height = buffer->GetHeight(  );

		buffer->Bind( );

		PreviewManager::PrepareRender( buffer );

		for ( auto mesh : model->GetMeshList( ) )
		{
			if ( rotating )
			{
				PreviewManager::UseRotatingTransformationMatrix( mesh );
			}
			else
			{
				PreviewManager::UseStaticTransformationMatrix( mesh );
			}
			
			PreviewManager::RenderMesh( mesh );
		}

		PreviewManager::FinishRender( );
	}

	void GenerateMaterialPreview( Pine::IAsset* asset, Editor::AssetPreviewGenerator::AssetPreview_t* preview, Pine::FrameBuffer* buffer )
	{
		static auto sphereModel = Pine::Assets::GetAsset<Pine::Model>( "Assets\\Engine\\Sphere.obj" );
		
		auto material = dynamic_cast< Pine::Material* >( asset );

		if (!material )
		{
			return;
		}

		if ( !preview->frameBuffer )
		{
			preview->frameBuffer = new Pine::FrameBuffer( );
			preview->frameBuffer->Create( 64, 64 );

			buffer = preview->frameBuffer;
		}

		Pine::RenderingConfiguration::Width = buffer->GetWidth( );
		Pine::RenderingConfiguration::Height = buffer->GetHeight( );

		buffer->Bind( );

		PreviewManager::PrepareRender( buffer );

		for ( auto mesh : sphereModel->GetMeshList(  ) )
		{
			PreviewManager::UseStaticTransformationMatrix( mesh );

			mesh->SetMaterial( material );
			
			PreviewManager::RenderMesh( mesh );
		}

		PreviewManager::FinishRender( );
	}

	Editor::AssetPreviewGenerator::AssetPreview_t* GenerateAssetPreview( Pine::IAsset* asset )
	{
		static auto unknownFileIcon = Pine::Assets::GetAsset<Pine::Texture2D>( "Assets\\Engine\\Icons\\028-delete file.png" );

		auto preview = Editor::AssetPreviewGenerator::GetAssetPreview( asset );

		if ( !preview )
		{
			preview = new Editor::AssetPreviewGenerator::AssetPreview_t( );
		}
		
		preview->asset = asset;
		preview->texture = unknownFileIcon->GetId( );

		if ( asset->GetType( ) == Pine::EAssetType::Texture2D )
		{
			GenerateTexturePreview( asset, preview );
		}

		if ( asset->GetType( ) == Pine::EAssetType::Model )
		{
			GenerateModelPreview( asset, preview, preview->frameBuffer, false );

			preview->texture = preview->frameBuffer->GetTextureId( );
			preview->invertUVs = true;
		}

		if ( asset->GetType(  ) == Pine::EAssetType::Material )
		{
			GenerateMaterialPreview( asset, preview, preview->frameBuffer );

			preview->texture = preview->frameBuffer->GetTextureId( );
			preview->invertUVs = true;
		}

		return preview;
	}

}

Editor::AssetPreviewGenerator::AssetPreview_t* Editor::AssetPreviewGenerator::GetAssetPreview( Pine::IAsset* asset )
{
	if ( m_AssetPreviews.count( asset ) == 0 )
	{
		return nullptr;
	}

	return m_AssetPreviews[ asset ];
}

bool Editor::AssetPreviewGenerator::GenerateFullscreenAssetPreview( Pine::IAsset* asset )
{
	auto preview = GetAssetPreview( asset );

	if ( !preview )
	{
		preview = new AssetPreview_t( );
	}

	if ( asset->GetType( ) == Pine::EAssetType::Texture2D )
	{
		GenerateTexturePreview( asset, preview );
		return false;
	}

	if ( asset->GetType( ) == Pine::EAssetType::Model )
	{
		GenerateModelPreview( asset, preview, g_FullscreenFrameBuffer, true );
		GenerateModelPreview( asset, preview, preview->frameBuffer, false );
		
		return true;
	}

	if ( asset->GetType( ) == Pine::EAssetType::Material )
	{
		GenerateMaterialPreview( asset, preview, g_FullscreenFrameBuffer );
		GenerateMaterialPreview( asset, preview, preview->frameBuffer );

		return true;
	}

	return false;
}

Pine::FrameBuffer* Editor::AssetPreviewGenerator::GetFullscreenFrameBuffer( )
{
	return g_FullscreenFrameBuffer;
}

void Editor::AssetPreviewGenerator::Setup( )
{
	g_FullscreenFrameBuffer = new Pine::FrameBuffer( );
	g_FullscreenFrameBuffer->Create( 128, 128 );
}

void Editor::AssetPreviewGenerator::Generate( )
{
	auto assets = Pine::Assets::GetAssets( );

	for ( auto& asset : assets )
	{
		m_AssetPreviews[ asset.second ] = GenerateAssetPreview( asset.second );
	}
}

void Editor::AssetPreviewGenerator::Dispose( )
{
	for (auto& preview : m_AssetPreviews )
	{

		if ( preview.second->frameBuffer )
		{
			preview.second->frameBuffer->Dispose( );
			delete preview.second->frameBuffer;
		}

		delete preview.second;
		
	}

	g_FullscreenFrameBuffer->Dispose( );
	delete g_FullscreenFrameBuffer;
}
