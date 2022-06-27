#pragma once
#include <cstdint>

namespace Pine
{

    enum Buffers
    {
        TextureBuffer = (1 << 0),
        DepthBuffer = (1 << 1),
        DepthStencilBuffer = (1 << 2),
        NormalBuffer= (1 << 3),
    };

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

        void Create( int width, int height, uint32_t buffers, bool multiSample = false );
        void Dispose( ) const;

		void BlitMultisample( const FrameBuffer* target ) const;
	};

}