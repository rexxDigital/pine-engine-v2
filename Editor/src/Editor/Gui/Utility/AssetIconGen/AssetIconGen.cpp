#include "AssetIconGen.hpp"
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

namespace
{

	struct AssetIcon_t
	{
		std::string m_Path;

		Pine::IAsset* m_Asset = nullptr;

		Pine::FrameBuffer* m_FrameBuffer = nullptr;
		Pine::Texture2D* m_FrameBufferTexture = nullptr;

		Pine::Texture2D* m_Texture2D = nullptr;

		bool m_LiveIconDirty = true;
		bool m_LiveIconReady = false;
	};

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

		void Setup( )
		{
			// We'll need a camera, a light, and the render target itself as entities to render the scene properly.
			// These entities obviously won't be part of the main entity list.

			// The camera
			m_CameraEntity = new Pine::Entity( 0 );
			m_CameraEntity->GetTransform( )->Position.z = -5.f; // This position would probably get adjusted in the future
			m_CameraEntity->AddComponent( new Pine::Camera( ) );
			m_Camera = m_CameraEntity->GetComponent<Pine::Camera>( );

			// The light
			m_LightEntity = new Pine::Entity( 0 );
			m_LightEntity->GetTransform( )->Rotation = glm::vec3( 0.f, 0.f, 1.f );
			m_LightEntity->AddComponent( new Pine::Light( ) );
			m_Light = m_CameraEntity->GetComponent<Pine::Light>( );

			// For the actor we'll just grab the transformation matrix, no other components required.
			m_Actor = new Pine::Entity( 0 );

			// Setup a new rendering context with the newly created camera.
			m_RenderingContext = new Pine::RenderingContext( );
			m_RenderingContext->m_Width = 64;
			m_RenderingContext->m_Height = 64;
			m_RenderingContext->m_Camera = m_Camera;
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
		icon->m_FrameBuffer->Create( 64, 64 );

		icon->m_FrameBufferTexture = new Pine::Texture2D;
		icon->m_FrameBufferTexture->CreateFromFrameBuffer( icon->m_FrameBuffer );
	}

}

Pine::Texture2D* Editor::Gui::Utility::AssetIconGen::GetAssetIcon( const std::string& str )
{
	if ( m_Icons.count( str ) == 0 )
	{
		return nullptr;
	}

	const auto icon = m_Icons[ str ].get( );

	if ( icon->m_LiveIconReady && icon->m_FrameBufferTexture )
	{
		return icon->m_FrameBufferTexture;
	}

	return icon->m_Texture2D;
}

Pine::Texture2D* Editor::Gui::Utility::AssetIconGen::GenerateAssetThumbnail( const std::string& str )
{
	if ( m_Icons.count( str ) == 0 )
	{
		return nullptr;
	}

	return m_Icons[ str ]->m_Texture2D;
}

void Editor::Gui::Utility::AssetIconGen::Update( )
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
			icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\code-file.png" );
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
				icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\cube_map.png" );
				break;
			case Pine::AssetType::Material:
				icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\image.png" );
				GenerateIconFrameBuffer( icon );
				break;
			case Pine::AssetType::Blueprint:
				icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\blueprint.png" );
				break;
			case Pine::AssetType::Level:
				icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\level.png" );
				break;
			case Pine::AssetType::Model:
				icon->m_Texture2D = Pine::Assets->GetAsset<Pine::Texture2D>( "Assets\\Editor\\Icons\\model.png" );
				GenerateIconFrameBuffer( icon );
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
}

void Editor::Gui::Utility::AssetIconGen::Render( )
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

	// Update all dirty/not ready icons

	for ( const auto& iconElement : m_Icons )
	{
		const auto icon = iconElement.second.get( );

		if ( icon->m_FrameBuffer == nullptr )
			continue;
		if ( !icon->m_LiveIconDirty && icon->m_LiveIconReady )
			continue;

		Pine::RenderManager->PrepareSceneRendering( );

		if ( icon->m_Asset->GetType( ) == Pine::AssetType::Material )
		{
			// We'll rendering the material on a sphere.
			static auto sphereModel = Pine::Assets->GetAsset<Pine::Model>( "Assets\\Engine\\Primitive Shapes\\sphere.fbx" );



		}

		Pine::RenderManager->FinishSceneRendering( );
	}


}
