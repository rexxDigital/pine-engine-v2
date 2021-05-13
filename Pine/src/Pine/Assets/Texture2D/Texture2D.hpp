#pragma once
#include "../IAsset/IAsset.hpp"

namespace Pine {

	class FrameBuffer;
	
	class Texture2D : public IAsset {
	private:
		unsigned int m_Id = 0;

		int m_Width = 0;
		int m_Height = 0;
	
		int m_Channels = 0;
	public:
		Texture2D( );

		unsigned int GetId( ) const;
		
		int GetWidth( ) const;
		int GetHeight( ) const;
		int GetChannels( ) const;

		void CreateFromFrameBuffer( Pine::FrameBuffer* frameBuffer ); // This should be fucking done the opposite way...
		void CreateFromData( int width, int height, int format, void* data );

		bool LoadFromFile( ) override;
		bool SaveToFile( ) override;
		void Dispose( ) override;
	};

}