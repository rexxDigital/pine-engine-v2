#pragma once
#include <string>
#include <glm/ext/vector_int2.hpp>

struct GLFWwindow;

namespace Pine::Window::Internal
{
	bool Create( );
	void Destroy( );

	GLFWwindow* GetWindowPointer( );

	void* GetWindowHWND( );
}

namespace Pine::Window
{
	void Show( );
	void Hide( );

	void Close( );

    void SetFullscreenMode( bool value );
	void SetPosition( int x, int y );
	void SetSize( int w, int h );
	void SetTitle( const std::string& str );

	// Quicker way of gettin size during rendering.
	void UpdateCachedSize( );
	glm::ivec2 GetCachedSize( );

	glm::ivec2 GetSize( );
    bool GetFullscreenMode( );
}