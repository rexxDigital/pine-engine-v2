#include "Entitylist.hpp"
#include "ImGui/imgui.h"
#include "Pine/Entity/Entity.hpp"
#include "Pine/Entitylist/EntityList.hpp"

namespace
{

	void DisplayEntity(Pine::Entity* e)
	{
		// Display children from here, call the same function.
		for (auto child : e->GetChildren())
		{
			DisplayEntity(child);
		}
		
		// Display the actual entity.

		
	}
	
}

void Editor::Gui::EntityList::Run()
{

	ImGui::Begin("Entity list", nullptr, 0);
	{

		for (auto entity : Pine::EntityList::GetEntities())
		{
			if (entity->GetParent() != nullptr)
			{
				continue;
			}

			DisplayEntity(entity);
		}
		
	}
	ImGui::End();
	
}
