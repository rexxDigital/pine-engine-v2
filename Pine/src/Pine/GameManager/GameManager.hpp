#pragma once
#include <string>

// Might have to implement this as an interface
namespace Pine::GameManager
{

    struct GameProperties
    {
        std::string Name = "";
        int Version = 0;
        std::string StartupLevel = "";
    };

    void Load( const std::string& filePath );
    void Save( const std::string& filePath );

    GameProperties* GetProperties( );

}