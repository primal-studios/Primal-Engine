#include "core/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <string>

std::shared_ptr<spdlog::logger> Log::sCoreLogger;
std::shared_ptr<spdlog::logger> Log::sClientLogger;

void Log::construct()
{
	const std::string patternString = "%^[%T], %t:%P - [%n]: [%l]%v%$";

	const auto coreFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("primal.log", true);
	const auto clientFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("client.log", true);

	const auto combinedFileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("console.log", true);

	coreFileSink->set_pattern(patternString);
	clientFileSink->set_pattern(patternString);
	combinedFileSink->set_pattern(patternString);

	sCoreLogger = spdlog::stdout_color_mt("Core");
	sCoreLogger->sinks().push_back(coreFileSink);
	sCoreLogger->sinks().push_back(combinedFileSink);
	sCoreLogger->set_pattern(patternString);
	sCoreLogger->set_level(spdlog::level::trace);
	sCoreLogger->flush_on(spdlog::level::trace);

	sClientLogger = spdlog::stdout_color_mt("Client");
	sClientLogger->sinks().push_back(clientFileSink);
	sClientLogger->sinks().push_back(combinedFileSink);
	sClientLogger->set_pattern(patternString);
	sClientLogger->set_level(spdlog::level::trace);
	sClientLogger->flush_on(spdlog::level::trace);
}