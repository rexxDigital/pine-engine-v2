#pragma once
#include <string>
#include <vector>

// Needed to manage strings in a nice way in Pine, so I made this simple utility namespace.
namespace Pine::String
{
	bool StartsWith(const std::string& str, const std::string& start);
	bool EndsWith(const std::string& str, const std::string& end);

	std::vector<std::string> Split(const std::string& str, const std::string& deli);
}