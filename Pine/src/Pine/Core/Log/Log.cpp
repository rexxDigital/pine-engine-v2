#include "Log.hpp"
#include <iostream>
#include <vector>

namespace Pine
{

	class CLog : public ILog
	{
	private:
		std::vector<std::pair<std::string, LogLevel>> m_ConsoleLogs;

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

		void AddToLogBuffer( const std::string& str, LogLevel level )
		{
			if ( m_ConsoleLogs.size( ) >= 256 )
			{
				m_ConsoleLogs.erase( m_ConsoleLogs.begin( ) );
			}

			m_ConsoleLogs.push_back( std::make_pair( str, level ) );
		}

	public:

		void Debug( const std::string& msg ) override
		{

            return;

//#ifdef _DEBUG
			SetConsoleColor( 8 );

			std::cout << "[Debug] ";

			std::cout << msg << std::endl;

			AddToLogBuffer( "[Debug] " + msg, LogLevel::Debug );

//#endif
		}

		void Message( const std::string& str ) override
		{
			SetConsoleColor( 15 );

			std::cout << "[Message] ";

			SetConsoleColor( 7 );

			std::cout << str << std::endl;

			AddToLogBuffer( "[Message] " + str, LogLevel::Message );

		}

		void Warning( const std::string& str ) override
		{
			SetConsoleColor( 14 );

			std::cout << "[Warning] ";

			SetConsoleColor( 7 );

			std::cout << str << std::endl;

			AddToLogBuffer( "[Warning] " + str, LogLevel::Warning );

		}

		void Error( const std::string& str ) override
		{
			SetConsoleColor( 12 );

			std::cout << "[Error] ";

			SetConsoleColor( 7 );

			std::cout << str << std::endl;

			AddToLogBuffer( "[Error] " + str, LogLevel::Error );
		}

		void Fatal( const std::string& str ) override
		{
			SetConsoleColor( 12 );

			std::cout << "[FATAL] ";
			std::cout << str << std::endl;

			AddToLogBuffer( "[FATAL] " + str, LogLevel::Fatal );
		}

		const std::vector<std::pair<std::string, LogLevel>>& GetLogMessages( ) override
		{
			return m_ConsoleLogs;
		}

		void ClearLogMessages( ) override
		{
			m_ConsoleLogs.clear( );
		}

	};

	ILog* CreateLogInterface( )
	{
		return new CLog( );
	}

}

