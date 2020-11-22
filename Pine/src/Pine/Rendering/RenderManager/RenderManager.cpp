#include "RenderManager.hpp"
#include <unordered_map>
#include <vector>
#include "../../Components/ModelRenderer/ModelRenderer.hpp"
#include "../../Entitylist/EntityList.hpp"
#include "../Renderer3D/Renderer3D.hpp"
#include "../../Components/Camera/Camera.hpp"
#include "../UniformBuffers/UniformBuffers.hpp"
#include "../../Components/Light/Light.hpp"
#include "../../Core/Log/Log.hpp"
#include "../../Core/Window/Window.hpp"
#include "../Skybox/Skybox.hpp"
#include <GL/glew.h>


#include "../../../ImGui/imgui_impl_glfw.h"
#include "../../../ImGui/imgui_impl_opengl3.h"

namespace {

	Pine::RenderingContext* g_RenderingContext = nullptr;
	
	Pine::RenderCallback g_PreRenderingCallback = nullptr;
	Pine::RenderCallback g_PostRenderingCallback = nullptr;

	bool VerifyRenderingContext(Pine::RenderingContext* context) {
		if (!context)
			return false;

		return true;
	}

	ImGuiContext* g_RenderingImGuiContext = nullptr;
	
}

void Pine::RenderManager::Run() {
	if (!VerifyRenderingContext(g_RenderingContext)) {
		return;
	}

	const bool hasFrameBuffer = g_RenderingContext->m_FrameBuffer != nullptr;

	// Setup frame buffer
	if (hasFrameBuffer) {
		g_RenderingContext->m_FrameBuffer->Bind();

		// Override rendering context's size variables.
		if (g_RenderingContext->m_AutoUpdateSize)
		{
			g_RenderingContext->m_Width = g_RenderingContext->m_FrameBuffer->GetWidth();
			g_RenderingContext->m_Height = g_RenderingContext->m_FrameBuffer->GetHeight();
		}
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	// Clear the buffers
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Reset the viewport size.
	glViewport(0, 0, 1600, 900);

	// Enable depth test just in case.
	glEnable(GL_DEPTH_TEST);

	if (g_RenderingContext->m_Camera == nullptr)
		return;

	if (g_PreRenderingCallback) {
		g_PreRenderingCallback();
	}

	// Better to keep this on the stack, since we want it empty the next frame anyway.
	// Also quicker!
	std::unordered_map<Pine::Model*, std::vector<Pine::Entity*>> renderBatch;
	std::vector<Pine::Light*> lights;

	for (auto& entity : EntityList::GetEntities()) {
		if (!entity->GetActive()) {
			continue;
		}

		for (auto& component : entity->GetComponents()) {
			if (!component->GetActive()) {
				continue;
			}

			component->OnRender();

			if (component->GetType() == EComponentType::ModelRenderer) {
				auto modelRenderer = dynamic_cast<Pine::ModelRenderer*>(component);
				auto model = modelRenderer->GetTargetModel();

				if (model != nullptr) {
					renderBatch[model].push_back(entity);
				}
			}
			else if (component->GetType() == EComponentType::Light) {
				lights.push_back(dynamic_cast<Pine::Light*>(component));
			}
		}
	}

	// Prepare data
	UniformBuffers::GetMatrixBufferData()->ProjectionMatrix = g_RenderingContext->m_Camera->GetProjectionMatrix();
	UniformBuffers::GetMatrixBufferData()->ViewMatrix = g_RenderingContext->m_Camera->GetViewMatrix();

	for (int i = 0; i < 5; i++) {
		UniformBuffers::GetLightsBufferData()->lights[i].color = glm::vec3(0.f, 0.f, 0.f);
		UniformBuffers::GetLightsBufferData()->lights[i].position = glm::vec3(0.f, 0.f, 0.f);
	}

	int processedLights = 0;

	for (auto light : lights) {
		if (processedLights >= 5) {
			Log::Warning("Too many dynamic lights in level.");
			break;
		}

		UniformBuffers::GetLightsBufferData()->lights[processedLights].position = light->GetParent()->GetTransform()->Position;
		UniformBuffers::GetLightsBufferData()->lights[processedLights].color = light->GetLightColor();

		processedLights++;
	}

	// Upload to the uniform buffer
	UniformBuffers::GetMatrixUniformBuffer()->Bind();
	UniformBuffers::GetMatrixUniformBuffer()->UploadData(0, sizeof(UniformBuffers::MatrixBufferData_t), UniformBuffers::GetMatrixBufferData());

	UniformBuffers::GetLightsUniformBuffer()->Bind();
	UniformBuffers::GetLightsUniformBuffer()->UploadData(0, sizeof(UniformBuffers::LightBufferData_t), UniformBuffers::GetLightsBufferData());

	UniformBuffers::GetMaterialUniformBuffer()->Bind();

	for (auto& renderItem : renderBatch) {
		for (auto& mesh : renderItem.first->GetMeshList()) {
			Renderer3D::PrepareMesh(mesh);
	
			for (auto entity : renderItem.second) {
				Renderer3D::RenderMesh(entity->GetTransform()->GetTransformationMatrix());
			}
		}
	}

	Skybox::Render();

	if (g_PostRenderingCallback) {
		g_PostRenderingCallback();
	}
}

void Pine::RenderManager::SetRenderingContext(RenderingContext* renderingContext) {
	g_RenderingContext = renderingContext;
}

Pine::RenderingContext* Pine::RenderManager::GetRenderingContext() {
	return g_RenderingContext;
}

void Pine::RenderManager::SetPreRenderingCallback(RenderCallback fn)
{
	g_PreRenderingCallback = fn;
}

void Pine::RenderManager::SetPostRenderingCallback(RenderCallback fn)
{
	g_PostRenderingCallback = fn;
}

void Pine::RenderManager::Setup()
{
	g_RenderingImGuiContext = ImGui::CreateContext();
}
