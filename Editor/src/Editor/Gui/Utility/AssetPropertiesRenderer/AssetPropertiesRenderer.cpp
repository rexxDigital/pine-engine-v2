#include "AssetPropertiesRenderer.hpp"
#include <Pine\Assets\IAsset\IAsset.hpp>
#include <Pine\Assets\Texture2D\Texture2D.hpp>
#include <ImGui\imgui.h>
#include <Pine\Assets\Material\Material.hpp>

#include "../../../ProjectManager/ProjectManager.hpp"
#include "..\..\Widgets\Widgets.hpp"
#include "Pine/Assets/Level/Level.hpp"
#include "Pine/Assets/Model/Model.hpp"

namespace {

	void RenderTexture2D(Pine::Texture2D* texture2d) {
		constexpr float padding = 256.f;

		auto avaliableArea = ImGui::GetContentRegionAvail();

		if (0.f > avaliableArea.x) {
			return;
		}

		ImGui::Image(reinterpret_cast<ImTextureID>(texture2d->GetId()), ImVec2(avaliableArea.x - padding, avaliableArea.x - padding));

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::BeginChild("Texture2DProperties", ImVec2(-1.f, 90.f), false);

		ImGui::Columns(2, nullptr, false);

		ImGui::Text("OpenGL ID");
		ImGui::Text("Width");
		ImGui::Text("Height");
		ImGui::Text("Channels");

		ImGui::NextColumn();

		ImGui::Text("%d", texture2d->GetId());
		ImGui::Text("%d", texture2d->GetWidth());
		ImGui::Text("%d", texture2d->GetHeight());

		ImGui::Text("%s", texture2d->GetChannels() == 3 ? "RGB" : "RGBA"); // we don't do any other channels ok

		ImGui::Columns(1);

		ImGui::EndChild();
	}

	void RenderMaterial(Pine::Material* mat) {
		if (auto newAsset = Editor::Gui::Widgets::AssetPicker("Diffuse", mat->GetDiffuse(), true, Pine::EAssetType::Texture2D)) {
			mat->SetDiffuse(dynamic_cast<Pine::Texture2D*>(newAsset));
		}

		if (auto newAsset = Editor::Gui::Widgets::AssetPicker("Specular Map", mat->GetSpecular(), true, Pine::EAssetType::Texture2D)) {
			mat->SetSpecular(dynamic_cast<Pine::Texture2D*>(newAsset));
		}

		Editor::Gui::Widgets::ColorPicker("Diffuse Color", mat->DiffuseColor());
		Editor::Gui::Widgets::ColorPicker("Specular Color", mat->SpecularColor());
		Editor::Gui::Widgets::ColorPicker("Ambient Color", mat->AmbientColor());

		ImGui::Columns(2, nullptr, false);
		ImGui::Text("Shininess");
		ImGui::NextColumn();
		ImGui::SetNextItemWidth(-1.f);
		float sh = mat->GetShininiess();
		if (ImGui::SliderFloat("##Shininiess", &sh, 1.f, 128.f, "%.1f", 1.f))
			mat->SetShininiess(sh);
		ImGui::Columns(1);

		if (auto newAsset = Editor::Gui::Widgets::AssetPicker("Shader", mat->GetShader(), true, Pine::EAssetType::Shader)) {
			mat->SetShader(dynamic_cast<Pine::Shader*>(newAsset));
		}
	}

	void RenderModel(Pine::Model* model)
	{
		for (int i = 0; i < model->GetMeshList().size(); i++)
		{
			if (ImGui::CollapsingHeader(std::string("Mesh #" + std::to_string(i)).c_str()))
			{
				auto mesh = model->GetMeshList()[i];

				ImGui::Columns(2, nullptr, false);

				ImGui::Text("Render Count");
				ImGui::Text("Has Element Buffer");

				ImGui::NextColumn();

				ImGui::Text("%d", mesh->GetRenderCount());
				ImGui::Text("%d", mesh->HasElementBuffer());

				ImGui::Columns(1);

				if (auto newAsset = Editor::Gui::Widgets::AssetPicker("Material", mesh->GetMaterial(), true, Pine::EAssetType::Material))
				{
					mesh->SetMaterial(dynamic_cast<Pine::Material*>(newAsset));
				}

				if (mesh->GetMaterial() && mesh->GetMaterial()->GetReadOnly())
				{
					if (ImGui::Button("Create Custom Material"))
					{

					}
				}
			}
		}
	}

	void RenderLevel(Pine::Level* lvl)
	{
		if (!lvl)
			return;

		ImGui::Text("Blueprints: %d", lvl->GetBlueprintCount());

		if (ImGui::Button("Load", ImVec2(300.f, 30.f)))
		{
			lvl->Load();
			Editor::ProjectManager::OpenLevel(lvl);
		}

		if (ImGui::Button("Save", ImVec2(300.f, 30.f)))
		{
			lvl->CreateFromCurrentLevel();
			Editor::ProjectManager::OpenLevel(lvl);
		}
	}

}

void Editor::Gui::Utility::AssetPropertiesRenderer::RenderAssetProperties(Pine::IAsset* asset) {
	if (!asset)
		return;

	switch (asset->GetType()) {
	case Pine::EAssetType::Texture2D:
		RenderTexture2D(dynamic_cast<Pine::Texture2D*>(asset));
		break;
	case Pine::EAssetType::Material:
		RenderMaterial(dynamic_cast<Pine::Material*>(asset));
		break;
	case Pine::EAssetType::Model:
		RenderModel(dynamic_cast<Pine::Model*>(asset));
		break;
	case Pine::EAssetType::Level:
		RenderLevel(dynamic_cast<Pine::Level*>(asset));
		break;
	default:
		break;
	}

}
