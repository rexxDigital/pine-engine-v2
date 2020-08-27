#pragma once

namespace Pine {

	class FrameBuffer {
	private:
		unsigned int m_Id = 0;

		unsigned int m_TextureBuffer = 0;
		unsigned int m_DepthBuffer = 0;
	public:

		FrameBuffer();

		unsigned int GetId() const;
		unsigned int GetTextureId() const;
		unsigned int GetDepthId() const;

		void Bind() const;
		void Unbind() const;

		void Dispose();

		void Create(int width, int height);

	};

}