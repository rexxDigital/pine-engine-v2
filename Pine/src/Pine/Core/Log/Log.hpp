#pragma once
#include <string>
#include <vector>
#include "../Interfaces/Interfaces.hpp"

namespace Pine
{

	enum class LogLevel
	{
		Debug, // Can be used for the pettiest things, because if something random were to break these logs could help work out how and where.
		Message, // Used for the more useful information to the user when actively using the engine.
		Warning, // Doesn't have to be bad, but good to tell the user that this is probably not something that should have happened.
		Error, // When something more important goes wrong, although the engine will continue working fine. 
		Fatal // When something fucked up bad that also causes the engine to "crash" and quit.
	};

	class ILog : public IInterface
	{
	public:
		// Log functions
		virtual void Debug( const std::string& str ) = 0;
		virtual void Message( const std::string& str ) = 0;
		virtual void Warning( const std::string& str ) = 0;
		virtual void Error( const std::string& str ) = 0;
		virtual void Fatal( const std::string& str ) = 0;

		// Can be used to render a console, stores all the log messages in a vector that will never be longer than 256 messages.

		virtual const std::vector<std::pair<std::string, LogLevel>>& GetLogMessages( ) = 0;
		virtual void ClearLogMessages( ) = 0;
		 
	};

}
