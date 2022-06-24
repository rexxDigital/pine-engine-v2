#include "AssetIcon.hpp"
#include <unordered_map>
#include <Pine/Assets/Assets.hpp>

#include "Pine/Components/Light/Light.hpp"
#include "Pine/Components/ModelRenderer/ModelRenderer.hpp"
#include "Pine/Entity/Entity.hpp"
#include "Pine/Rendering/Renderer3D/Renderer3D.hpp"
#include "Pine/Rendering/RenderingContext/RenderingContext.hpp"
#include "Pine/Rendering/RenderManager/RenderManager.hpp"
#include "Pine/OpenGL/FrameBuffer/FrameBuffer.hpp"
#include "Pine/Components/Components.hpp"
#include "Pine/Core/String/String.hpp"

#include "../../Gui.hpp"
#include "Pine/Rendering/Skybox/Skybox.hpp"

using namespace Editor::Gui::Utility::AssetIcon;

namespace
{
	std::unordered_map<std::string, std::unique_ptr<AssetIcon_t>> m_Icons;

	// To render the "live" icon, we'll need some fake entities
	// to pass onto the rendering system.
	namespace LiveScene
	{
		Pine::RenderingContext* m_RenderingContext = nullptr;

		Pine::Entity* m_CameraEntity = nullptr;
		Pine::Camera* m_Camera = nullptr;

		Pine::Entity* m_LightEntity = nullptr;
		Pine::Light* m_Light = nullptr;

		Pine::Entity* m_Actor = nullptr;

		// The thumbnail will be rendererd in the properties panel with a bigger preview (256x256),
		// otherwise it's the same as the icons for any other asset.
		Pine::FrameBuffer* m_ThumbnailFrameBuffer = nullptr;
		Pine::Texture2D* m_ThumbnailTexture = nullptr;

		void Setup( )
		{
			// We'll need a camera, a light, and the render target itself as entities to render the scene properly.
			// These entities obviously won't be part of the main entity list.

			// The camera
			m_CameraEntity = new Pine::Entity( 0 );
			m_CameraEntity->AddComponent( new Pine::Camera( ) );
			m_CameraEntity->GetTransform( )->Position.z = 0.f;
			m_Camera = m_CameraEntity->GetComponent<Pine::Camera>( );

			// The light
			m_LightEntity = new Pine::Entity( 0 );
			m_LightEntity->GetTransform( )->Rotation = glm::vec3( 0.f, 0.f, 1.f );
			m_LightEntity->AddComponent( new Pine::Light( ) );
			m_Light = m_LightEntity->GetComponent<Pine::Light>( );

			// For the actor we'll just grab the transformation matrix, no other components required.
			m_Actor = new Pine::Entity( 0 );

			// Setup a new rendering context with the newly created camera.
			m_RenderingContext = new Pine::RenderingContext( );
			m_RenderingContext->m_Camera = m_Camera;
			m_RenderingContext->m_Width = 64;
			m_RenderingContext->m_Height = 64;
			m_RenderingContext->m_ClearColor = glm::vec4( 0.06f, 0.06f, 0.06f, 1.f );

			m_ThumbnailFrameBuffer = new Pine::FrameBuffer( );
			m_ThumbnailFrameBuffer->Create( 256, 256, true );

			m_ThumbnailTexture = new Pine::Texture2D( );
			m_ThumbnailTexture->CreateFromFrameBuffer( m_ThumbnailFrameBuffer );
		}

		// I'll just urge myself to call this instead of changing it myself,
		// to avoid camera issues that only happen when we do all of this "manually".
		// Should maybe make the rendering pipeline easier to use or something.
		void SetFrameBuffer( Pine::FrameBuffer* frameBuffer )
		{
			m_RenderingContext->m_FrameBuffer = frameBuffer;

			m_RenderingContext->m_Width = frameBuffer->GetWidth( );
			m_RenderingContext->m_Height = frameBuffer->GetHeight( );

			m_Camera->OnRender( );
		}

		void UpdateEntities( )
		{
			m_Camera->OnRender( );
			m_Actor->GetTransform( )->OnRender( );
		}

	}

	void GenerateIconFrameBuffer( AssetIcon_t* icon )
	{
		if ( icon->m_FrameBuffer != nullptr )
			return;

		icon->m_FrameBuffer = new Pine::FrameBuffer;
		icon->m_FrameBuffer->Create( 64, 64, true );

		icon->m_FrameBufferTexture = new Pine::Texture2D;
		icon->m_FrameBufferTexture->CreateFromFrameBuffer( icon->m_FrameBuffer );
	}

	void RenderMaterialPreview( AssetIcon_t* icon )
	{
		// We'll rendering the material on a sphere.
		static auto sphereModel = Pine::Assets->GetAsset<Pine::Model>( "Assets/Engine/Primitive Shapes/sphere.fbx" );

		const auto material = dynamic_cast< Pine::Material* >( icon->m_Asset );
		if ( !material )
			return;

		// Setup the actor's position for a sphere
		LiveScene::m_Actor->GetTransform( )->Position = glm::vec3( 0.f, 0.f, 2.0f );
		LiveScene::UpdateEntities( );

		// Render it
		Pine::Renderer3D->PrepareMeshRendering( );
		Pine::Renderer3D->PrepareMesh( sphereModel->GetMeshList( )[ 0 ], material );
		Pine::Renderer3D->RenderMesh( LiveScene::m_Actor->GetTransform( )->GetTransformationMatrix( ) );

		// Mark as ready
		icon->m_LiveIconReady = true;
		icon->m_LiveIconDirty = false;
	}

	void RenderModelPreview( AssetIcon_t* icon )
	{
		const auto model = dynamic_cast< Pine::Model* >( icon->m_Asset );
		if ( !model )
			return;

		// Work out the size of the model
		glm::vec3 lowestBounds( 0.f );
		glm::vec3 highestBounds( 0.f );

		// Since we only have the bounding boxes for each mess we'll do this madness.
		for ( const auto& mesh : model->GetMeshList( ) )
		{
			const auto& maxs = mesh->GetMins( );
			const auto& mins = mesh->GetMaxs( );

			lowestBounds.x = std::min( mins.x, lowestBounds.x );
			lowestBounds.y = std::min( mins.y, lowestBounds.y );
			lowestBounds.z = std::min( mins.z, lowestBounds.z );

			highestBounds.x = std::max( maxs.x, highestBounds.x );
			highestBounds.y = std::max( maxs.y, highestBounds.y );
			highestBounds.z = std::max( maxs.z, highestBounds.z );
		}

		const glm::vec3 size = highestBounds + glm::abs( lowestBounds );

		// Setup the actor's positon accordingly
		LiveScene::m_Actor->GetTransform( )->Position.y = -( size.y / 2.f );
		LiveScene::m_Actor->GetTransform( )->Position.z = size.x * 1.5f;
		LiveScene::m_Actor->GetTransform( )->Rotation.y = 00.f;

		LiveScene::UpdateEntities( );

		// Render each mesh
		Pine::Renderer3D->PrepareMeshRendering( );

		for ( auto& mesh : model->GetMeshList( ) )
		{
			Pine::Renderer3D->PrepareMesh( mesh );
			Pine::Renderer3D->RenderMesh( LiveScene::m_Actor->GetTransform( )->GetTransformationMatrix( ) );
		}

		// Mark as ready
		icon->m_LiveIconReady = true;
		icon->m_LiveIconDirty = false;
	}

}

AssetIcon_t* Editor::Gui::Utility::AssetIcon::GetAssetIcon( const std::string& str )
{
	if ( m_Icons.count( str ) == 0 )
	{
		return nullptr;
	}

	return m_Icons[ str ].get( );
}

Pine::Texture2D* Editor::Gui::Utility::AssetIcon::GetThumbnailTexture( )
{
	return LiveScene::m_ThumbnailTexture;
}

void Editor::Gui::Utility::AssetIcon::Update( )
{
	std::vector<std::string> removeList;

	// Remove unloaded icons
	for ( auto& iconElement : m_Icons )
	{
		const auto& icon = iconElement.second;

		// If the asset exists, don't dispose the icon.
		if ( Pine::Assets->GetAsset( icon->m_Path ) != nullptr )
			continue;

		// Dispose FBO texture
		delete icon->m_FrameBufferTexture;

		// Dispose frame buffer
		if ( icon->m_FrameBuffer )
		{
			icon->m_FrameBuffer->Dispose( );
			delete icon->m_FrameBuffer;
		}

		removeList.push_back( iconElement.first );
	}

	for ( auto& rem : removeList )
		m_Icons.erase( rem );

	// Loop through all assets, and add an icon for each of them.
	for ( auto& assetElement : Pine::Assets->GetAssets( ) )
	{
		const auto& asset = assetElement.second;

		AssetIcon_t* icon = nullptr;

		// Check if icon exists
		if ( m_Icons.count( asset->GetPath( ).string( ) ) != 0 )
		{
			icon = m_Icons[ asset->GetPath( ).string( ) ].get( );
		}

		// Not found, add the new icon for the asset
		if ( icon == nullptr )
		{
			auto newIcon = std::make_unique<AssetIcon_t>( );

			newIcon->m_Asset = asset;
			newIcon->m_Path = asset->GetPath( ).string( );

			m_Icons[ newIcon->m_Path ] = std::move( newIcon );

			icon = m_Icons[ asset->GetPath( ).string( ) ].get( );
		}

		if ( asset->GetType( ) == Pine::AssetType::Invalid && Pine::String::EndsWith( icon->m_Path, ".dll" ) )
		{
			icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets/Editor/Icons/code-file.png" );
		}
		else
		{
			// Update the asset icon, depending on it's type:
			switch ( asset->GetType( ) )
			{
			case Pine::AssetType::Texture2D:
				icon->m_Texture2D = dynamic_cast< Pine::Texture2D* >( asset );
				break;
			case Pine::AssetType::Texture3D:
				icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets/Editor/Icons/cube_map.png" );
				break;
			case Pine::AssetType::Material:
				icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets/Editor/Icons/image.png" );
				GenerateIconFrameBuffer( icon );
				break;
			case Pine::AssetType::Blueprint:
				icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets/Editor/Icons/blueprint.png" );
				break;
			case Pine::AssetType::Level:
				icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets/Editor/Icons/level.png" );
				break;
			case Pine::AssetType::Model:
				icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets/Editor/Icons/model.png" );
				GenerateIconFrameBuffer( icon );
				break;
			case Pine::AssetType::Shader:
				icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets/Editor/Icons/shader.png" );
				break;
			case Pine::AssetType::Script:
				icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets/Editor/Icons/code-file.png" );
				break;
			case Pine::AssetType::Terrain:
				icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets/Editor/Icons/terrain.png" );
				break;
			default:
				icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets/Editor/Icons/corrupt.png" );
				break;
			}
		}
	}
}

void Editor::Gui::Utility::AssetIcon::Render( )
{
	if ( LiveScene::m_RenderingContext == nullptr )
	{
		LiveScene::Setup( );
	}

	// Mark the selected asset as dirty every frame
	if ( !Globals::SelectedAssetPtrs.empty( ) )
	{
		const auto selectedAsset = Globals::SelectedAssetPtrs[ 0 ];

		m_Icons[ selectedAsset->GetPath( ).string( ) ]->m_LiveIconDirty = true;
	}

	// Prepare rendering context
	const auto oldRenderingContext = Pine::RenderManager->GetRenderingContext( );

	Pine::RenderManager->SetRenderingContext( LiveScene::m_RenderingContext );

	// Update light
	LiveScene::m_LightEntity->GetTransform( )->Rotation = glm::vec3( 45.f, 45.f, -65.f );

	Pine::Renderer3D->ResetLightData( );
	Pine::Renderer3D->PrepareLightData( LiveScene::m_Light );
	Pine::Renderer3D->UploadLightData( );

	// Update all dirty/not ready icons
	for ( const auto& iconElement : m_Icons )
	{
		const auto icon = iconElement.second.get( );

		if ( icon->m_FrameBuffer == nullptr )
			continue;
		if ( !icon->m_LiveIconDirty && icon->m_LiveIconReady )
			continue;
		if ( !icon->m_Asset )
			continue;

		LiveScene::SetFrameBuffer( icon->m_FrameBuffer );

		Pine::RenderManager->PrepareSceneRendering( );

		if ( icon->m_Asset->GetType( ) == Pine::AssetType::Material )
		{
			RenderMaterialPreview( icon );
		}

		if ( icon->m_Asset->GetType( ) == Pine::AssetType::Model )
		{
			RenderModelPreview( icon );
		}

		Pine::RenderManager->FinishSceneRendering( );
	}

	// Render a thumbnail for the selected asset.
	if ( !Globals::SelectedAssetPtrs.empty( ) )
	{
		const auto selectedAsset = Globals::SelectedAssetPtrs[ 0 ];
		const auto icon = m_Icons[ selectedAsset->GetPath( ).string( ) ].get(  );

		LiveScene::SetFrameBuffer( LiveScene::m_ThumbnailFrameBuffer );

		Pine::RenderManager->PrepareSceneRendering( );

		if ( selectedAsset->GetType(  ) == Pine::AssetType::Material )
		{
			RenderMaterialPreview( icon );
		}

		if ( selectedAsset->GetType( ) == Pine::AssetType::Model )
		{
			RenderModelPreview( icon );
		}

		Pine::RenderManager->FinishSceneRendering( );
	}

	Pine::RenderManager->SetRenderingContext( oldRenderingContext );
}
