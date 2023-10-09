#pragma once
#include "spdlog/spdlog.h"

enum LogLevel : int
{
	Debug = spdlog::level::debug,
	Info = spdlog::level::info,
};

class Log
{
public:
	static void Init(LogLevel level, std::string fileName);

	static void Info(std::string format, std::string args...);
	static void WInfo(std::wstring format, std::wstring args...);
};
