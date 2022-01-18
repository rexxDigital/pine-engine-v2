#pragma once

namespace Pine
{

	class FrameBuffer
	{
	private:
		unsigned int m_Id = 0;

		unsigned int m_TextureBuffer = 0;
		unsigned int m_NormalBuffer = 0;
		unsigned int m_DepthStencilBuffer = 0;

		int m_Width = 0;
		int m_Height = 0;
	public:
		FrameBuffer( );

		unsigned int GetId( ) const;
		unsigned int GetTextureId( ) const;
		unsigned int GetNormalBufferId( ) const;
		unsigned int GetDepthId( ) const;

		int GetWidth( ) const;
		int GetHeight( ) const;

		void Bind( ) const;
		void Unbind( ) const;

		void Dispose( ) const;

		void Create( int width, int height, bool createNormal = false, bool multiSample = false );

		void BlitMultisample( FrameBuffer* target );
	};

}