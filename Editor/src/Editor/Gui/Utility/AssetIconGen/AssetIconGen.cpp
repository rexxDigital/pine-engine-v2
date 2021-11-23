#include "AssetIconGen.hpp"
#include <unordered_map>
#include <Pine\Assets\Assets.hpp>



#include "Pine/Components/Light/Light.hpp"
#include "Pine/Components/ModelRenderer/ModelRenderer.hpp"
#include "Pine/Entity/Entity.hpp"
#include "Pine/Rendering/Renderer3D/Renderer3D.hpp"
#include "Pine/Rendering/RenderingContext/RenderingContext.hpp"
#include "Pine/Rendering/RenderManager/RenderManager.hpp"
#include "Pine\OpenGL\FrameBuffer\FrameBuffer.hpp"
#include "Pine/Components/Components.hpp"

namespace {

	struct AssetIcon_t {
		std::string m_Path = "";

		Pine::IAsset* m_Asset = nullptr;
		Pine::FrameBuffer* m_FrameBuffer = nullptr;

		Pine::Texture2D* m_Texture2D = nullptr;
	};

	// I couldn't care less about disposing this memory later.

	// I don't really like this at all... what if we have a lot of entities in the future?
	Pine::Entity* m_FakeCameraEntity;
	Pine::Entity* m_FakeModelEntity;
	Pine::Entity* m_FakeLightEntity;
	Pine::RenderingContext* m_AssetPreviewContext;

	Pine::Texture2D* m_AssetPreviewFrameBufferTexture;
	Pine::FrameBuffer* m_AssetPreviewFrameBuffer;
	
	std::unordered_map<std::string, std::unique_ptr<AssetIcon_t>> m_Icons;

	void SetupAssetPreviewRenderingContext( )
	{
		// Setup fake entities
		m_FakeCameraEntity = new Pine::Entity( 0 );
		m_FakeCameraEntity->RegisterComponent( Pine::Components->CreateComponent( Pine::ComponentType::Camera, true ) );

		m_FakeModelEntity = new Pine::Entity( 0 );
		m_FakeModelEntity->RegisterComponent( Pine::Components->CreateComponent( Pine::ComponentType::ModelRenderer, true ) );

		m_FakeLightEntity = new Pine::Entity( 0 );
		m_FakeLightEntity->RegisterComponent( Pine::Components->CreateComponent( Pine::ComponentType::Light, true ) );
		
		// Setup rendering context
		m_AssetPreviewContext = new Pine::RenderingContext;

		m_AssetPreviewContext->m_Width = 256;
		m_AssetPreviewContext->m_Height = 256;
		m_AssetPreviewContext->m_Is3D = true;
		m_AssetPreviewContext->m_Camera = m_FakeCameraEntity->GetComponent<Pine::Camera>( );

		m_AssetPreviewFrameBuffer = new Pine::FrameBuffer;
		m_AssetPreviewFrameBuffer->Create( 256, 256 );

		m_AssetPreviewFrameBufferTexture = new Pine::Texture2D;
		m_AssetPreviewFrameBufferTexture->CreateFromFrameBuffer( m_AssetPreviewFrameBuffer );
	}

}

Pine::Texture2D* Editor::Gui::Utility::AssetIconGen::GetAssetIcon( const std::string& str ) {
	if ( m_Icons.count( str ) == 0 ) {
		return nullptr;
	}

	return m_Icons[ str ]->m_Texture2D;
}

Pine::Texture2D* Editor::Gui::Utility::AssetIconGen::GenerateAssetThumbnail( const std::string& str )
{
	if ( m_Icons.count( str ) == 0 ) {
		return nullptr;
	}

	auto& assetItem = m_Icons[ str ];

	// Prepare the asset rendering context if we have to.
	if ( m_AssetPreviewContext == nullptr )
	{
		SetupAssetPreviewRenderingContext( );
	}

	// This function should only be called once per frame by the editor whenever it wants to render
	// a thumbnail for the asset. As a result, I couldn't care less and will just re-render it every time
	// this function is called.

	m_AssetPreviewContext->m_FrameBuffer = m_AssetPreviewFrameBuffer;
	
	if ( assetItem->m_Asset->GetType( ) == Pine::AssetType::Model )
	{
		// Prepare rendering context
		const auto oldRenderingCtxPtr = Pine::RenderManager->GetRenderingContext( );

		// We still have to do some "low level" stuff to render stuff this way.
		Pine::RenderManager->SetRenderingContext( m_AssetPreviewContext );
		Pine::RenderManager->PrepareSceneRendering( );

		Pine::Renderer3D->ResetLightData( );
		Pine::Renderer3D->PrepareLightData( m_FakeLightEntity->GetComponent<Pine::Light>(  ) );
		Pine::Renderer3D->UploadLightData( );
		
		Pine::Renderer3D->PrepareMeshRendering( );

		// Render the mesh itself
		

		// Render the scene to our frame buffer
		Pine::RenderManager->FinishSceneRendering( );
		
		// Restore rendering context
		Pine::RenderManager->SetRenderingContext( oldRenderingCtxPtr );
	}

	return m_AssetPreviewFrameBufferTexture;
}

void Editor::Gui::Utility::AssetIconGen::Update( ) {
	std::vector<std::string> removeList;

	// Remove unloaded icons
	for ( auto& iconElement : m_Icons ) {
		auto& icon = iconElement.second;

		// If the asset exists, don't dispose the icon.
		if ( Pine::Assets->GetAsset( icon->m_Path ) != nullptr )
			continue;

		// Dispose frame buffer
		if ( icon->m_FrameBuffer ) {
			icon->m_FrameBuffer->Dispose( );
			delete icon->m_FrameBuffer;
		}

		removeList.push_back( iconElement.first );
	}

	for ( auto& rem : removeList )
		m_Icons.erase( rem );

	// Loop through all assets, and add an icon for each of them.
	for ( auto& assetElement : Pine::Assets->GetAssets( ) ) {
		auto& asset = assetElement.second;

		AssetIcon_t* icon = nullptr;

		// Check if icon exists
		if ( m_Icons.count( asset->GetPath( ).string( ) ) != 0 ) {
			icon = m_Icons[ asset->GetPath( ).string( ) ].get( );
		}

		// Not found, add the new icon for the asset
		if ( icon == nullptr ) {
			auto newIcon = std::make_unique<AssetIcon_t>( );

			newIcon->m_Asset = asset;
			newIcon->m_Path = asset->GetPath( ).string( );

			m_Icons[ newIcon->m_Path ] = std::move( newIcon );

			icon = m_Icons[ asset->GetPath( ).string( ) ].get( );
		}

		// Update the asset icon, depending on it's type:
		switch ( asset->GetType( ) ) {
		case Pine::AssetType::Texture2D:
			icon->m_Texture2D = dynamic_cast< Pine::Texture2D* >( asset );
			break;
		case Pine::AssetType::Texture3D:
			icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\cube_map.png" );
			break;
		case Pine::AssetType::Material:
			icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\image.png" );
			break;
		case Pine::AssetType::Blueprint:
			icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\blueprint.png" );
			break;
		case Pine::AssetType::Level:
			icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\level.png" );
			break;
		case Pine::AssetType::Model:
			icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\model.png" );
			break;
		case Pine::AssetType::Shader:
			icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\shader.png" );
			break;
		case Pine::AssetType::Script:
			icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\code-file.png" );
			break;
		case Pine::AssetType::Terrain:
			icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\terrain.png" );
			break;
		default:
			icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\corrupt.png" );
			break;
		}

	}
}
