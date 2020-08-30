#include "PreviewManager.hpp"


#include <glm/ext/matrix_transform.hpp>
#include <GL/glew.h>
#include <Pine/Entity/Entity.hpp>
#include <Pine/Components/Camera/Camera.hpp>
#include <Pine/OpenGL/FrameBuffer/FrameBuffer.hpp>
#include <Pine/UniformBuffers/UniformBuffers.hpp>

namespace {
	Pine::FrameBuffer* g_FrameBuffer = nullptr;

	Pine::Entity* g_FakeCameraEntity = nullptr;

	Pine::Camera* g_FakeCamera = nullptr;

	glm::mat4 g_EntityTransformationMatrix = glm::mat4(1.f);
}

void Pine::PreviewManager::Setup() {

	// Create fake camera
	g_FakeCameraEntity = new Entity(0);
	g_FakeCameraEntity->AddComponent(new Pine::Camera());
	g_FakeCameraEntity->GetTransform()->Position.z = -2.f;

	g_FakeCamera = reinterpret_cast<Pine::Camera*>(g_FakeCameraEntity->GetComponents()[1]);
	g_FakeCamera->OnSetup();
	g_FakeCamera->OnRender();

	g_EntityTransformationMatrix = glm::mat4(1.f);
	g_EntityTransformationMatrix = glm::translate(g_EntityTransformationMatrix, glm::vec3(0.f, 0.f, 0.f));
}

void Pine::PreviewManager::PrepareRender(FrameBuffer* frameBuffer) {
	g_FrameBuffer = frameBuffer;
	g_FrameBuffer->Bind();

	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, frameBuffer->GetWidth(), frameBuffer->GetHeight());

	glEnable(GL_DEPTH_TEST);

	// Prepare data
	UniformBuffers::GetMatrixBufferData()->ProjectionMatrix = g_FakeCamera->GetProjectionMatrix();
	UniformBuffers::GetMatrixBufferData()->ViewMatrix = g_FakeCamera->GetViewMatrix();

	for (int i = 0; i < 5; i++) {
		UniformBuffers::GetLightsBufferData()->lights[i].color = glm::vec3(0.f, 0.f, 0.f);
		UniformBuffers::GetLightsBufferData()->lights[i].position = glm::vec3(0.f, 0.f, 0.f);
	}

	UniformBuffers::GetLightsBufferData()->lights[0].color = glm::vec3(1.f, 1.f, 1.f);
	UniformBuffers::GetLightsBufferData()->lights[0].position = glm::vec3(0.f, 0.f, -100.f);

	// Upload to the uniform buffer
	UniformBuffers::GetMatrixUniformBuffer()->Bind();
	UniformBuffers::GetMatrixUniformBuffer()->UploadData(0, sizeof(UniformBuffers::MatrixBufferData_t), UniformBuffers::GetMatrixBufferData());

	UniformBuffers::GetLightsUniformBuffer()->Bind();
	UniformBuffers::GetLightsUniformBuffer()->UploadData(0, sizeof(UniformBuffers::LightBufferData_t), UniformBuffers::GetLightsBufferData());

	UniformBuffers::GetMaterialUniformBuffer()->Bind();
}

void Pine::PreviewManager::FinishRender() {
//	Skybox::Render();

	// Reset the frame buffer for rendering.
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Pine::PreviewManager::Dispose() {
	delete g_FakeCameraEntity;
}

glm::mat4& Pine::PreviewManager::GetRenderTransformationMatrix() {
	return g_EntityTransformationMatrix;
}
