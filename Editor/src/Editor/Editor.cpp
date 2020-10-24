#include "Editor.hpp"
#include "Gui\Gui.hpp"
#include "ProjectManager\ProjectManager.hpp"

void Editor::Setup( ) {

	Editor::ProjectManager::Setup( );
	Editor::Gui::Setup( );

}
