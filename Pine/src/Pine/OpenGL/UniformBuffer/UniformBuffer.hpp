#pragma once
#include <cstdint>

namespace Pine
{

	class UniformBuffer
	{
	private:
		uint32_t m_Id = 0;
		int m_Binding = 0;
	public:

		void Create( int size, int binding );

		void Bind( ) const;

		void Dispose( );

		uint32_t GetId( ) const;
		int GetBinding( ) const;

		void UploadData( int offset, int size, void* data );
	};

}