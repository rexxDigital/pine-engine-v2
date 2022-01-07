#include "UniformBuffers.hpp"

namespace
{

	// Data
	Pine::UniformBuffers::MatrixBufferData_t* g_MatrixBufferData = nullptr;
	Pine::UniformBuffers::LightBufferData_t* g_LightsBufferData = nullptr;
	Pine::UniformBuffers::MaterialBufferData_t* g_MaterialBufferData = nullptr;
	Pine::UniformBuffers::TransformData_t* g_TransformBufferData = nullptr;

	// Uniform Buffers
	Pine::UniformBuffer* g_MatrixUniformBuffer = nullptr;
	Pine::UniformBuffer* g_LightsUniformBuffer = nullptr;
	Pine::UniformBuffer* g_MaterialUniformBuffer = nullptr;
	Pine::UniformBuffer* g_TransformUniformBuffer = nullptr;

}

Pine::UniformBuffers::MatrixBufferData_t* Pine::UniformBuffers::GetMatrixBufferData( )
{
	return g_MatrixBufferData;
}

Pine::UniformBuffer* Pine::UniformBuffers::GetMatrixUniformBuffer( )
{
	return g_MatrixUniformBuffer;
}

void Pine::UniformBuffers::Setup( )
{
	g_MatrixBufferData = new MatrixBufferData_t( );
	g_LightsBufferData = new LightBufferData_t( );
	g_MaterialBufferData = new MaterialBufferData_t( );
	g_TransformBufferData = new TransformData_t( );

	g_MatrixUniformBuffer = new UniformBuffer( );
	g_MatrixUniformBuffer->Create( sizeof( MatrixBufferData_t ), 0 );

	g_LightsUniformBuffer = new UniformBuffer( );
	g_LightsUniformBuffer->Create( sizeof( LightBufferData_t ), 1 );

	g_MaterialUniformBuffer = new UniformBuffer( );
	g_MaterialUniformBuffer->Create( sizeof( MaterialBufferData_t ), 2 );

	g_TransformUniformBuffer = new UniformBuffer( );
	g_TransformUniformBuffer->Create( sizeof( TransformData_t ), 3 );
}

void Pine::UniformBuffers::Dispose( )
{
	g_MatrixUniformBuffer->Dispose( );
	g_LightsUniformBuffer->Dispose( );
	g_MaterialUniformBuffer->Dispose( );
	g_TransformUniformBuffer->Dispose( );

	delete g_MatrixUniformBuffer;
	delete g_LightsUniformBuffer;
	delete g_MaterialUniformBuffer;
	delete g_TransformUniformBuffer;
	
	delete g_LightsBufferData;
	delete g_MatrixBufferData;
	delete g_MaterialBufferData;
	delete g_TransformBufferData;
}

Pine::UniformBuffers::LightBufferData_t* Pine::UniformBuffers::GetLightsBufferData( )
{
	return g_LightsBufferData;
}

Pine::UniformBuffer* Pine::UniformBuffers::GetLightsUniformBuffer( )
{
	return g_LightsUniformBuffer;
}

Pine::UniformBuffers::MaterialBufferData_t* Pine::UniformBuffers::GetMaterialBufferData( )
{
	return g_MaterialBufferData;
}

Pine::UniformBuffers::TransformData_t* Pine::UniformBuffers::GetTransformBufferData( )
{
	return g_TransformBufferData;
}

Pine::UniformBuffer* Pine::UniformBuffers::GetMaterialUniformBuffer( )
{
	return g_MaterialUniformBuffer;
}

Pine::UniformBuffer* Pine::UniformBuffers::GetTransformDataUniformBuffer( )
{
	return g_TransformUniformBuffer;
}
