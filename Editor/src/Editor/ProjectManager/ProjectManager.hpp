#pragma once
#include <Pine\Assets\Level\Level.hpp>
#include <string>

namespace Editor::ProjectManager {

	void Setup( );
	void Save( );

	void CreateProject( const std::string& directory );
	void OpenProject( const std::string& directory );

	void ReloadProjectAssets( );

	bool HasProjectOpen( );
	std::string GetCurrentProjectDirectory( );

	void OpenLevel( const std::string& path );
	void OpenLevel( Pine::Level* level );
	void SaveLevel( const std::string& path );
	Pine::Level* GetCurrentLevel( );


	const std::vector<std::string>& GetAvaliableProjects( );

}