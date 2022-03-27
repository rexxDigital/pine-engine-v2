#include "GameManager.hpp"
#include "../Core/Serialization/Serialization.hpp"
#include "../Core/Log/Log.hpp"
#include "../Assets/Assets.hpp"
#include "../Assets/Level/Level.hpp"
#include "../Core/Window/Window.hpp"
#include <filesystem>
#include <memory>
#include <fstream>

namespace
{
    // Couldn't give 2 fucks about freeing this memory, OS will take care of that.
    auto properties = new Pine::GameManager::GameProperties;
}

void Pine::GameManager::Load( const std::string& filePath )
{
    if ( !std::filesystem::exists( filePath ) )
    {
        Log->Error("Failed to load game file, file does not exist.");
        return;
    }

    const auto json = Serialization::LoadJSONFromFile( filePath );

    if ( json.contains( "name" ) )
        properties->Name = json["name"];
    if ( json.contains( "version" ) )
        properties->Version = json["version"];
    if ( json.contains( "startupLevel" ) )
        properties->StartupLevel = json["startupLevel"];

    // Attempt to load startup level
    auto level = Assets->GetAsset<Pine::Level>(properties->StartupLevel);

    if (level && level->GetType() == AssetType::Level)
    {
        level->Load();
    }

    if (properties->Name != "")
    {
        Pine::Window::SetTitle(properties->Name);
    }
}

void Pine::GameManager::Save( const std::string& filePath )
{
    nlohmann::json j;

    j["name"] = properties->Name;
    j["version"] = properties->Version;
    j["startupLevel"] = properties->StartupLevel;

    std::ofstream stream(filePath);

    stream << j;

    stream.close();
}

Pine::GameManager::GameProperties* Pine::GameManager::GetProperties( )
{
    return properties;
}
