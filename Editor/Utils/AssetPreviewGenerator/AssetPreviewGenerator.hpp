#pragma once
#include <string>

namespace Pine {
	class FrameBuffer;
	class IAsset;
}

namespace Editor::AssetPreviewGenerator
{
	struct AssetPreview_t
	{
		Pine::IAsset* asset = nullptr;
		Pine::FrameBuffer* frameBuffer = nullptr;

		unsigned int texture = 0;
		bool invertUVs = false;
	};

	AssetPreview_t* GetAssetPreview( Pine::IAsset* asset );

	bool GenerateFullscreenAssetPreview( Pine::IAsset* asset );

	Pine::FrameBuffer* GetFullscreenFrameBuffer( );
	
	void Setup( );
	void Generate( );
	void Dispose( );

}
