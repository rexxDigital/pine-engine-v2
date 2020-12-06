#pragma once
#include <vector>

namespace Pine
{

	class VertexArray
	{
	private:
		uint32_t m_Id = 0;
		std::vector<uint32_t> m_Buffers;

		uint32_t CreateBuffer( );
	public:
		VertexArray( ) = default;

		void Create( );
		void Bind( ) const;
		void Dispose( );

		void DisposeBuffer( uint32_t buffer );

		uint32_t StoreFloatBuffer( const std::vector<float> buffer, const int vectorSize, const int binding );
		uint32_t StoreIntBuffer( const std::vector<int> buffer );
		
		uint32_t GetId( ) const;
	};

}