#pragma once
#include <string>
#include <vector>

namespace Pine::Log {

	enum class LogLevel
	{
		Debug, // Can be used for the pettiest things, because if something random were to break these logs could help work out how and where.
		Message, // Used for the more useful information to the user when actively using the engine.
		Warning, // Doesn't have to be bad, but good to tell the user that this is probably not something that should have happened.
		Error, // When something more important goes wrong, although the engine will continue working fine. 
		Fatal // When something fucked up bad that also causes the engine to "crash" and quit.
	};

	// Log functions

	void Debug( const std::string& str );
	void Message( const std::string& str );
	void Warning( const std::string& str );
	void Error( const std::string& str );
	void Fatal( const std::string& str );

	// Can be used to render a console, stores all the log messages in a vector that will never be longer than 256 messages.

	const std::vector<std::pair<std::string, LogLevel>>& GetLogMessages( );
	void ClearLogMessages( );

}