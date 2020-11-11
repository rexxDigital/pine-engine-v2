#pragma once
#include <Pine\Assets\Level\Level.hpp>
#include <string>

namespace Editor::ProjectManager {

	void Setup( );
	void Save( );

	void CreateProject( const std::string& directory );
	void OpenProject( const std::string& directory );

	bool HasProjectOpen( );
	std::string GetCurrentProjectDirectory( );

	Pine::Level* GetCurrentLevel( );

	const std::vector<std::string>& GetAvaliableProjects( );

}