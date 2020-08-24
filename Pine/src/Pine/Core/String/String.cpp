#include "String.hpp"

bool Pine::String::StartsWith(const std::string& str, const std::string& start)
{
	// Does this need it's own function? Yes, StartsWith is cleaner than that.
	return str.rfind(start, 0) == 0;
}

bool Pine::String::EndsWith(const std::string& str, const std::string& end)
{
	if (end.size() > str.size()) return false;
	return std::equal(end.rbegin(), end.rend(), str.rbegin());
}

std::vector<std::string> Pine::String::Split(const std::string& str, const std::string& delim)
{
	// https://stackoverflow.com/a/37454181

	std::vector<std::string> tokens;
	size_t prev = 0, pos = 0;
	do
	{
		pos = str.find(delim, prev);
		if (pos == std::string::npos) pos = str.length();
		std::string token = str.substr(prev, pos - prev);
		if (!token.empty()) tokens.push_back(token);
		prev = pos + delim.length();
	} while (pos < str.length() && prev < str.length());
	return tokens;
}

