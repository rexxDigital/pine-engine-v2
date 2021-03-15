#include "ProjectManager.hpp"
#include <filesystem>
#include <json.hpp>
#include <Pine\Assets\Assets.hpp>
#include <Pine\Core\Window\Window.hpp>

#include "Pine/ScriptManager/ScriptManager.hpp"

// TODO: Store project meta data, and verify stuff.
// This will probably never get done, lol.

namespace {

	Pine::Level* g_CurrentLevel = nullptr;

	bool g_ProjectOpen = false;
	std::string g_CurrentProject = "";

	std::vector<std::string> g_AvaliableProjects;

	void LoadProject( const std::string& directory ) {
		g_CurrentProject = directory;
		g_ProjectOpen = true;

		Pine::Assets::LoadFromDirectory( directory );

		Pine::Window::SetSize( 1024, 768 );
		Pine::Window::UpdateCachedSize( );

		Pine::Window::SetTitle( "Pine - " + g_CurrentProject );
	}

}

void Editor::ProjectManager::Setup( ) {

	for ( const auto& dirEntry : std::filesystem::directory_iterator( "Projects" ) ) {
		if ( !dirEntry.is_directory( ) )
			continue;

		g_AvaliableProjects.push_back( dirEntry.path( ).string( ) );
	}

}

void Editor::ProjectManager::Save( ) {
	if ( g_CurrentLevel )
	{
		g_CurrentLevel->CreateFromCurrentLevel( );
		g_CurrentLevel->SetUpdated( true );
	}

	Pine::Assets::SaveAssets( );
}

bool Editor::ProjectManager::HasProjectOpen( ) {
	return g_ProjectOpen;
}

std::string Editor::ProjectManager::GetCurrentProjectDirectory( ) {
	return g_CurrentProject;
}

Pine::Level* Editor::ProjectManager::GetCurrentLevel( ) {
	return g_CurrentLevel;
}

void Editor::ProjectManager::OpenLevel( const std::string& path )
{
	auto level = Pine::Assets::GetAsset<Pine::Level>( path );
	if ( !level )
		return;

	level->Load( );

	g_CurrentLevel = level;
}

void Editor::ProjectManager::OpenLevel( Pine::Level* level )
{
	g_CurrentLevel = level;
}

void Editor::ProjectManager::SaveLevel( const std::string& path )
{
	g_CurrentLevel = new Pine::Level( );

	g_CurrentLevel->CreateFromCurrentLevel( );
	g_CurrentLevel->SetFilePath( path );

	g_CurrentLevel->SaveToFile( );
}

void Editor::ProjectManager::OpenProject( const std::string& directory ) {
	if ( !std::filesystem::exists( directory ) )
		return;

	LoadProject( directory );
}

void Editor::ProjectManager::CreateProject( const std::string& directory ) {
	if ( std::filesystem::exists( directory ) )
		return;

	std::filesystem::create_directory( directory );

	LoadProject( directory );
}

const std::vector<std::string>& Editor::ProjectManager::GetAvaliableProjects( ) {
	return g_AvaliableProjects;
}

void Editor::ProjectManager::ReloadProjectAssets( ) {
	if ( !g_ProjectOpen )
		return;
	
	auto level = new Pine::Level;

	level->CreateFromCurrentLevel( );
	
	Pine::Assets::LoadFromDirectory( GetCurrentProjectDirectory( ) );

	Pine::ScriptingManager::CompileScripts( );

	level->Load( );
	level->Dispose( );

	delete level;
}
