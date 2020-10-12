#include "UniformBuffers.hpp"

namespace {

	// Data
	Pine::UniformBuffers::MatrixBufferData_t* g_MatrixBufferData = nullptr;
	Pine::UniformBuffers::LightBufferData_t* g_LightsBufferData = nullptr;
	Pine::UniformBuffers::MaterialBufferData_t* g_MaterialBufferData = nullptr;

	// Uniform Buffers
	Pine::UniformBuffer* g_MatrixUniformBuffer = nullptr;
	Pine::UniformBuffer* g_LightsUniformBuffer = nullptr;
	Pine::UniformBuffer* g_MaterialUniformBuffer = nullptr;

}

Pine::UniformBuffers::MatrixBufferData_t* Pine::UniformBuffers::GetMatrixBufferData() {
	return g_MatrixBufferData;
}

Pine::UniformBuffer* Pine::UniformBuffers::GetMatrixUniformBuffer() {
	return g_MatrixUniformBuffer;
}

void Pine::UniformBuffers::Setup() {
	g_MatrixBufferData = new Pine::UniformBuffers::MatrixBufferData_t();
	g_LightsBufferData = new Pine::UniformBuffers::LightBufferData_t();
	g_MaterialBufferData = new Pine::UniformBuffers::MaterialBufferData_t();

	g_MatrixUniformBuffer = new Pine::UniformBuffer();
	g_MatrixUniformBuffer->Create(sizeof(Pine::UniformBuffers::MatrixBufferData_t), 0);

	g_LightsUniformBuffer = new Pine::UniformBuffer();
	g_LightsUniformBuffer->Create(sizeof(Pine::UniformBuffers::LightBufferData_t), 1);

	g_MaterialUniformBuffer = new Pine::UniformBuffer();
	g_MaterialUniformBuffer->Create(sizeof(Pine::UniformBuffers::MaterialBufferData_t), 2);
}

void Pine::UniformBuffers::Dispose() {
	g_MatrixUniformBuffer->Dispose();
	g_LightsUniformBuffer->Dispose();
	g_MaterialUniformBuffer->Dispose();

	delete g_MatrixUniformBuffer;
	delete g_LightsUniformBuffer;
	delete g_MaterialUniformBuffer;

	delete g_LightsBufferData;
	delete g_MatrixBufferData;
	delete g_MaterialBufferData;
}

Pine::UniformBuffers::LightBufferData_t* Pine::UniformBuffers::GetLightsBufferData() {
	return g_LightsBufferData;
}

Pine::UniformBuffer* Pine::UniformBuffers::GetLightsUniformBuffer() {
	return g_LightsUniformBuffer;
}

Pine::UniformBuffers::MaterialBufferData_t* Pine::UniformBuffers::GetMaterialBufferData() {
	return g_MaterialBufferData;
}

Pine::UniformBuffer* Pine::UniformBuffers::GetMaterialUniformBuffer() {
	return g_MaterialUniformBuffer;
}
