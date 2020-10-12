#pragma once
#include <vector>

namespace Pine {
	class Entity;
	class Asset;
}

namespace Editor::Gui {

	enum class SelectedItemType {
		None,
		Entity,
		Asset
	};

	enum class GizmoMovementType {
		Move,
		Rotate,
		Scale
	};

	// Global variables aint evil
	namespace Globals {

		// Manage selected entity/asset

		inline SelectedItemType SelectedItem = SelectedItemType::None;

		inline std::vector<Pine::Entity*> SelectedEntityPtrs;
		inline std::vector<Pine::Asset*> SelectedAssetPtrs;

		inline GizmoMovementType SelectedGizmoMovementType = GizmoMovementType::Move;

	}

	void Setup( );

}