#pragma once
#include <vector>

struct ImFont;

namespace Pine {
	class Entity;
	class IAsset;
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

		inline bool IsInLevelView = false;
		inline bool IsHoveringLevelView = false;
        inline bool IsUsingGizmo = false;
        inline bool IgnorePickInput = false;

        inline bool IsGameFocused = false;

		// Manage selected entity/asset

		inline SelectedItemType SelectedItem = SelectedItemType::None;

		inline std::vector<Pine::Entity*> SelectedEntityPtrs;
		inline std::vector<Pine::IAsset*> SelectedAssetPtrs;

		inline GizmoMovementType SelectedGizmoMovementType = GizmoMovementType::Move;

	}

	namespace Fonts {

		inline ImFont* TitleFont = nullptr;
		inline ImFont* BoldFont = nullptr;
		inline ImFont* CodeFont = nullptr;

	}

	void Setup( );

}