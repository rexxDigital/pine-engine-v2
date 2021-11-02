#include "Log.hpp"
#include <iostream>
#include <vector>
#include <Windows.h>

namespace
{

	std::vector<std::pair<std::string, Pine::Log::LogLevel>> m_ConsoleLogs;

#ifdef _WIN32

	void SetConsoleColor( int colorCode )
	{
		static auto consoleHandle = GetStdHandle( STD_OUTPUT_HANDLE );
		SetConsoleTextAttribute( consoleHandle, colorCode );
	}

#else

	void SetConsoleColor( int colorCode )
	{
		// TODO: Do something for other operating systems, not that I really support them right now but you get the idea.
	}

#endif

	void AddToLogBuffer( const std::string& str, Pine::Log::LogLevel level )
	{
		if ( m_ConsoleLogs.size( ) >= 256 )
		{
			m_ConsoleLogs.erase( m_ConsoleLogs.begin( ) );
		}

		m_ConsoleLogs.push_back( std::make_pair( str, level ) );
	}

}

void Pine::Log::Debug( const std::string& msg )
{
#ifdef _DEBUG
	SetConsoleColor( 8 );

	std::cout << "[Debug] ";

	std::cout << msg << std::endl;

	AddToLogBuffer( "[Debug] " + msg, LogLevel::Debug );

#endif
}

void Pine::Log::Message( const std::string& str ) {
	SetConsoleColor( 15 );

	std::cout << "[Message] ";

	SetConsoleColor( 7 );

	std::cout << str << std::endl;

	AddToLogBuffer( "[Message] " + str, LogLevel::Message );

}

void Pine::Log::Warning( const std::string& str ) {
	SetConsoleColor( 14 );

	std::cout << "[Warning] ";

	SetConsoleColor( 7 );

	std::cout << str << std::endl;

	AddToLogBuffer( "[Warning] " + str, LogLevel::Warning );

}

void Pine::Log::Error( const std::string& str ) {
	SetConsoleColor( 12 );

	std::cout << "[Error] ";

	SetConsoleColor( 7 );

	std::cout << str << std::endl;

	AddToLogBuffer( "[Error] " + str, LogLevel::Error );
}

void Pine::Log::Fatal( const std::string& str ) {
	SetConsoleColor( 12 );

	std::cout << "[FATAL] ";
	std::cout << str << std::endl;

	AddToLogBuffer( "[FATAL] " + str, LogLevel::Fatal );
}

const std::vector<std::pair<std::string, Pine::Log::LogLevel>>& Pine::Log::GetLogMessages( )
{
	return m_ConsoleLogs;
}

void Pine::Log::ClearLogMessages( )
{
	m_ConsoleLogs.clear( );
}

