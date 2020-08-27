#include <iostream>
#include <vector>

#include <Pine/Pine.hpp>
#include <Pine/Renderer3D/Renderer3D.hpp>
#include <Pine/Entity/Entity.hpp>
#include <Pine/Entitylist/Entitylist.hpp>
#include <Pine/Components/ModelRenderer/ModelRenderer.hpp>
#include <Pine/Components/Camera/Camera.hpp>
#include <Pine/RenderManager/RenderManager.hpp>
#include <Pine/Components/Light/Light.hpp>

Pine::Entity* entity = nullptr;

void SetupSampleScene()
{
	auto model = Pine::Assets::GetAsset<Pine::Model>("Assets\\bunny.obj");
	auto mesh = model->GetMeshList()[0];

	mesh->GetMaterial()->AmbientColor() = glm::vec3(0.3f, 0.3f, 0.3f);

	mesh->GetMaterial()->SpecularColor() = glm::vec3(1.f, 1.f, 1.f);
	mesh->GetMaterial()->DiffuseColor() = glm::vec3(0.5f, 0.5f, 0.5f);
	mesh->GetMaterial()->SetShininiess(16.f);

	entity = Pine::EntityList::CreateEntity("Test Entity");

	entity->AddComponent(new Pine::ModelRenderer());
	entity->GetComponent<Pine::ModelRenderer>()->SetTargetModel(model);
	entity->GetTransform()->Position.y = -0.1f;

	auto camera = Pine::EntityList::CreateEntity("Camera");
	
	camera->AddComponent(new Pine::Camera());
	camera->GetTransform()->Position.z = -.3f;

	Pine::RenderManager::SetCamera(camera->GetComponent<Pine::Camera>());

	auto light = Pine::EntityList::CreateEntity("light");

	light->AddComponent(new Pine::Light());
	light->GetTransform()->Position = glm::vec3(0.f, 0.f, -20.f);

}

void HandleRendering()
{
	entity->GetTransform()->Rotation.y += 0.5f;
}

int main()
{
	if (!Pine::Setup())
	{
		return 1;
	}

	Pine::Assets::LoadFromDirectory("Assets");

	SetupSampleScene();

	Pine::SetRenderingCallback(HandleRendering);

	Pine::Run();

	Pine::Terminate();
	
	return 0;
}
