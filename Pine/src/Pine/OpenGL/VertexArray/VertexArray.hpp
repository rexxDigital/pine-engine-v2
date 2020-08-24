#pragma once
#include <vector>

namespace Pine
{

	class VertexArray
	{
	private:
		uint32_t m_Id = 0;
		std::vector<uint32_t> m_Buffers;
	
		uint32_t CreateBuffer();
	public:
		VertexArray() = default;

		void Create();
		void Bind() const;
		void Dispose();

		void StoreFloatBuffer(const std::vector<float> buffer, const int vectorSize, const int binding);
		void StoreIntBuffer(const std::vector<int> buffer);

		uint32_t GetId() const;
	};

}