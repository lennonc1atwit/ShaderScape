#pragma once
#include <string>

class Logger
{
public:
	enum Severity
	{
		Info,
		Warn,
		Fail
	};

	static void LogMessageConsole(Severity type, std::string filename, int lineNum, std::string message)
	{
		char icon;
		switch (type)
		{
		case Logger::Info: icon = Logger::INFO_ICON; break;
		case Logger::Warn:icon = Logger::WARN_ICON; break;
		case Logger::Fail:icon = Logger::FAIL_ICON; break;
		default: icon = ' '; break;
		}
		// filename.substr(filename.find_last_of("\\")
		printf(Logger::LOG_FORMAT, icon, filename.c_str(), lineNum, message.c_str());
	}
private:
	Logger()
	{

	}
	~Logger()
	{

	}
	inline static const char INFO_ICON = '*';
	inline static const char WARN_ICON = '-';
	inline static const char FAIL_ICON = '!';
	inline static const char* LOG_FORMAT = "[%c] %s %d: %s";
};