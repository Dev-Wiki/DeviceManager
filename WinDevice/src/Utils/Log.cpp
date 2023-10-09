#include "Log.h"
#include "spdlog/spdlog.h"


void Log::Init(LogLevel level, std::string fileName)
{
	spdlog::set_pattern("[%H:%M:%S %z] [%n] [%^---%L---%$] [thread %t] %v");
}

void Log::Info(std::string format, std::string args...)
{
	spdlog::info(format, args);
}

void Log::WInfo(std::wstring format, std::wstring args...)
{
	std::string s_format(format.begin(), format.end());

}