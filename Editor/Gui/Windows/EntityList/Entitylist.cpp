#include "Entitylist.hpp"
#include "ImGui/imgui.h"
#include "Pine/Entity/Entity.hpp"
#include "Pine/Entitylist/EntityList.hpp"
#include "../../Gui.hpp"

#define GLFW_KEY_ESCAPE 256

namespace
{
	bool g_OpenedEntityContextMenu = false;

	bool g_IsRenamingEntity = false;
	Pine::Entity* g_RenamingEntityPointer = nullptr; // inb4 entity gets removed while changing name /shrug
	char g_RenameBuffer[64];

	void ShowEntityContextMenu(Pine::Entity* e) {
		const bool hasEntity = e != nullptr;
		std::string renderText = "EntityContextMenu";

		if (hasEntity)
			renderText += std::to_string(e->GetId());

		// Show the entity context menu
		if (ImGui::BeginPopupContextItem(renderText.c_str())) {
			g_OpenedEntityContextMenu = true;

			if (hasEntity)
				Editor::Gui::SetSelectedEntity(e);
			
			if (ImGui::MenuItem("Create New Entity")) {
				auto newEnt = Pine::EntityList::CreateEntity("Entity");

				g_IsRenamingEntity = true;
				g_RenamingEntityPointer = newEnt;

				strcpy_s(g_RenameBuffer, newEnt->GetName().c_str());

				Editor::Gui::SetSelectedEntity(newEnt);
			}

			ImGui::Separator();

			if (ImGui::MenuItem("Create child", nullptr, nullptr, hasEntity)) {
				auto newEnt = e->CreateChild();

				newEnt->SetName("Child");

				g_IsRenamingEntity = true;
				g_RenamingEntityPointer = newEnt;

				strcpy_s(g_RenameBuffer, newEnt->GetName().c_str());
			}

			if (ImGui::MenuItem("Rename", nullptr, nullptr, hasEntity)) {
				g_IsRenamingEntity = true;
				g_RenamingEntityPointer = e;

				strcpy_s(g_RenameBuffer, e->GetName().c_str());
			}

			if (ImGui::MenuItem("Remove", nullptr, nullptr, hasEntity)) {
				Pine::EntityList::DeleteEntity(e);
				Editor::Gui::SetSelectedEntity(nullptr);
			}

			ImGui::EndPopup();
		}
	}

	void DisplayEntity(Pine::Entity* e)
	{
		const auto& children = e->GetChildren();

		const bool selected = Editor::Gui::GetSelectedEntity() == e;
		const bool renamingEntity = (g_IsRenamingEntity && g_RenamingEntityPointer == e);
		const std::string renderText = e->GetName() + "##" + std::to_string(e->GetId());

		// Display the actual entity in the "normal" way.
		if (children.empty() || renamingEntity) {
			if (renamingEntity) {
				ImGui::SetKeyboardFocusHere();

				if (ImGui::InputText("##NewEntityName", g_RenameBuffer, 64, ImGuiInputTextFlags_::ImGuiInputTextFlags_EnterReturnsTrue)) {
					g_IsRenamingEntity = false;
					g_RenamingEntityPointer->SetName(g_RenameBuffer);
				}

				if (ImGui::IsKeyPressed(GLFW_KEY_ESCAPE)) {
					g_IsRenamingEntity = false;
				}
			}
			else {
				if (ImGui::Selectable(renderText.c_str(), selected)) {
					Editor::Gui::SetSelectedEntity(e);
				}

				ShowEntityContextMenu(e);
			}
		}
		else {
			ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

			if (selected)
				flags |= ImGuiTreeNodeFlags_Selected;

			if (ImGui::TreeNodeEx(renderText.c_str(), flags)) {
				ShowEntityContextMenu(e);

				if (ImGui::IsItemClicked()) {
					Editor::Gui::SetSelectedEntity(e);
				}

				for (auto child : e->GetChildren()) {
					DisplayEntity(child);
				}

				ImGui::TreePop();
			}
			else {
				ShowEntityContextMenu(e);
			}
		}
	}
}

void Editor::Gui::EntityList::Run()
{

	ImGui::Begin("Entity list", nullptr, 0);
	{
		g_OpenedEntityContextMenu = false;
		
		ImGui::BeginChild("##EntityFrame", ImVec2(-1.f, -1.f), false);

		const auto& entities = Pine::EntityList::GetEntities();

		for (int i = 0; i < entities.size();i++)
		{
			auto entity = entities[i];

			if (entity->GetParent() != nullptr)
			{
				continue;
			}

			DisplayEntity(entity);
		}
		
		ImGui::EndChild();

		if (!g_OpenedEntityContextMenu)
			ShowEntityContextMenu(nullptr);

	}
	ImGui::End();
	
}
