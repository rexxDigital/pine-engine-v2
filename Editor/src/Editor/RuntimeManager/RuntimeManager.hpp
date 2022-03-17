#pragma once
#include <string>

#include "Pine/RuntimeLoader/RuntimeLoader.hpp"

namespace Editor::RuntimeManager
{

    const std::string& GetRuntimeLibraryPath( );
    void SetRuntimeLibraryPath( const std::string& path );

    void LoadRuntimeLibrary( );
    void UnloadRuntimeLibrary( );

    // Returns a pointer to the module if loaded,
    // nullptr if not loaded.
    Pine::ModuleHandle* GetProjectRuntimeLibrary( );

    // Simple placeholder for auto compile feature

    const std::string& GetRuntimeSourcePath( );
    void SetRuntimeSourcePath( const std::string& path );

    void SetRuntimeAutoCompile( bool enabled );
    bool GetRuntimeAutoCompile( );

    bool GetRuntimeCompileAvailable( );

    bool CompileRuntime( );

    void Setup( );
    void Update( );

}