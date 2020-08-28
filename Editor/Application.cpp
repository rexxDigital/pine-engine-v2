#include <iostream>
#include <Pine/Pine.hpp>
#include <Pine/Gui/Gui.hpp>
#include "Gui/Gui.hpp"
#include <Pine/Components/ModelRenderer/ModelRenderer.hpp>
#include <Pine/Entitylist/EntityList.hpp>
#include <Pine/Components/Camera/Camera.hpp>
#include <Pine/RenderManager/RenderManager.hpp>
#include <Pine/Components/Light/Light.hpp>

Pine::Entity* entity = nullptr;

void OnRender() {

}

void OnRenderGui() {

    Editor::Gui::Run();

}

void SetupSampleScene() {
	Pine::Assets::LoadFromDirectory("Assets");

	auto model = Pine::Assets::GetAsset<Pine::Model>("Assets\\cube.obj");
	auto mesh = model->GetMeshList()[0];

	mesh->GetMaterial()->AmbientColor() = glm::vec3(0.3f, 0.3f, 0.3f);

	mesh->GetMaterial()->SpecularColor() = glm::vec3(1.f, 1.f, 1.f);
	mesh->GetMaterial()->DiffuseColor() = glm::vec3(0.5f, 0.5f, 0.5f);
	mesh->GetMaterial()->SetShininiess(16.f);

	entity = Pine::EntityList::CreateEntity("Test Entity");

	entity->AddComponent(new Pine::ModelRenderer());
	entity->GetComponent<Pine::ModelRenderer>()->SetTargetModel(model);
	entity->GetTransform()->Position.y = -2.f;

	auto camera = Pine::EntityList::CreateEntity("Camera");

	camera->AddComponent(new Pine::Camera());
	camera->GetTransform()->Position.z = -5.f;

	Pine::RenderManager::SetCamera(camera->GetComponent<Pine::Camera>());

	auto light = Pine::EntityList::CreateEntity("light");

	light->AddComponent(new Pine::Light());
	light->GetTransform()->Position = glm::vec3(0.f, 0.f, -20.f);
}

int main()
{
    if (!Pine::Setup()) {
        return 1;
    }

    Pine::Window::SetSize(1280, 720);
    Editor::Gui::Setup();

	SetupSampleScene();
	
	// Setup rendering callbacks
    Pine::SetRenderingCallback(OnRender);
    Pine::Gui::SetGuiRenderCallback(OnRenderGui);

    // Setup frame buffer.
    Pine::SetFrameBuffer(Editor::Gui::GetViewportFrameBuffer());
	
    Pine::Run();

    Editor::Gui::Dispose();
	
    Pine::Terminate();

    return 0;
}