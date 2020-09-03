#include "RenderManager.hpp"
#include <unordered_map>
#include <vector>
#include "../../Components/ModelRenderer/ModelRenderer.hpp"
#include "../../Entitylist/EntityList.hpp"
#include "../Renderer3D/Renderer3D.hpp"
#include "../../Components/Camera/Camera.hpp"
#include "../../UniformBuffers/UniformBuffers.hpp"
#include "../../Components/Light/Light.hpp"
#include "../../Core/Log/Log.hpp"
#include "../../Core/Window/Window.hpp"

namespace {

	Pine::Camera* g_TargetCamera = nullptr;

}

void Pine::RenderManager::Run() {
	if (g_TargetCamera == nullptr) {
		return;
	}

	RenderingConfiguration::Width = Window::GetCachedSize( ).x;
	RenderingConfiguration::Height = Window::GetCachedSize( ).y;
	
	// Better to keep this on the stack, since we want it empty the next frame anyway.
	// Also quicker!
	std::unordered_map<Pine::Model*, std::vector<Pine::Entity*>> renderBatch;
	std::vector<Pine::Light*> lights;

	// This code is probably not efficient at all, please fix.
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
	UniformBuffers::GetMatrixBufferData()->ProjectionMatrix = g_TargetCamera->GetProjectionMatrix();
	UniformBuffers::GetMatrixBufferData()->ViewMatrix = g_TargetCamera->GetViewMatrix();

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
}

void Pine::RenderManager::SetCamera(Camera* camera) {
	g_TargetCamera = camera;
}

Pine::Camera* Pine::RenderManager::GetCamera()
{
	return g_TargetCamera;
}
