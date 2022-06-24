#include "Window.hpp"
#include "../Log/Log.hpp"


#include <GLFW/glfw3.h>
#include <cassert>


#include <GLFW/glfw3.h>

namespace
{
    GLFWwindow* m_Window = nullptr;

    bool m_FullscreenMode = false;

    glm::ivec2 g_CachedWindowSize;
}

bool Pine::Window::Internal::Create( )
{
    glfwDefaultWindowHints( );

    glfwWindowHint( GLFW_VISIBLE, GLFW_FALSE );
    glfwWindowHintString(GLFW_X11_CLASS_NAME, "floating_window");

    m_Window = glfwCreateWindow( 1280, 720, "Pine", m_FullscreenMode ? glfwGetPrimaryMonitor() : nullptr, nullptr );
    if ( m_Window == nullptr )
    {
        Log->Fatal( "Failed to create window." );
        return false;
    }

    glfwMakeContextCurrent( m_Window );
    glfwSwapInterval( 1 );

    return true;
}

void Pine::Window::Internal::Destroy( )
{
    glfwDestroyWindow( m_Window );
}

GLFWwindow* Pine::Window::Internal::GetWindowPointer( )
{
    return m_Window;
}

void* Pine::Window::Internal::GetWindowHWND( )
{
    return nullptr;
    //return glfwGetWin32Window( m_Window );
}

void Pine::Window::Show( )
{
    assert( m_Window != nullptr );

    glfwShowWindow( m_Window );
}

void Pine::Window::Hide( )
{
    assert( m_Window != nullptr );

    glfwHideWindow( m_Window );
}

void Pine::Window::SetPosition( int x, int y )
{
    glfwSetWindowPos( m_Window, x, y );
}

void Pine::Window::SetSize( int w, int h )
{
    glfwSetWindowSize( m_Window, w, h );
}

void Pine::Window::SetTitle( const std::string& str )
{
    glfwSetWindowTitle( m_Window, str.c_str( ) );
}

void Pine::Window::UpdateCachedSize( )
{
    g_CachedWindowSize = GetSize( );
}

glm::ivec2 Pine::Window::GetCachedSize( )
{
    return g_CachedWindowSize;
}

glm::ivec2 Pine::Window::GetSize( )
{
    glm::ivec2 vec;

    glfwGetWindowSize( m_Window, &vec.x, &vec.y );

    return vec;
}

void Pine::Window::Close( )
{
    glfwSetWindowShouldClose( m_Window, GLFW_TRUE );
}

void Pine::Window::SetFullscreenMode( bool value )
{
    m_FullscreenMode = value;
}

bool Pine::Window::GetFullscreenMode( )
{
    return m_FullscreenMode;
}
