#include "core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <string>

std::shared_ptr<spdlog::logger> Log::mCoreLogger;
std::shared_ptr<spdlog::logger> Log::mClientLogger;

void Log::construct()
{
	const std::string patternString = "%^[%T], %t:%P - [%n]: [%l]%v%$";

	const auto coreFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("../primal.log", true);
	const auto clientFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("../client.log", true);

	const auto combinedFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("../console.log", true);

	coreFileSink->set_pattern(patternString);
	clientFileSink->set_pattern(patternString);
	combinedFileSink->set_pattern(patternString);

	mCoreLogger = spdlog::stdout_color_mt("Core");
	mCoreLogger->sinks().push_back(coreFileSink);
	mCoreLogger->sinks().push_back(combinedFileSink);
	mCoreLogger->set_pattern(patternString);
	mCoreLogger->set_level(spdlog::level::trace);
	mCoreLogger->flush_on(spdlog::level::trace);

	mClientLogger = spdlog::stdout_color_mt("Client");
	mClientLogger->sinks().push_back(clientFileSink);
	mClientLogger->sinks().push_back(combinedFileSink);
	mClientLogger->set_pattern(patternString);
	mClientLogger->set_level(spdlog::level::trace);
	mClientLogger->flush_on(spdlog::level::trace);
}