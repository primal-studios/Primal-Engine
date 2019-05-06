#ifndef log_h__
#define log_h__

#pragma warning(push)
#pragma warning(disable: 4251)

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#include <memory>

class Log
{
public:
	static void construct();

	inline static std::shared_ptr<spdlog::logger>& getCoreLogger()
	{
		return sCoreLogger;
	}

	inline static std::shared_ptr<spdlog::logger>& getClientLogger()
	{
		return sClientLogger;
	}

private:
	static std::shared_ptr<spdlog::logger> sCoreLogger;
	static std::shared_ptr<spdlog::logger> sClientLogger;
};

// Core log macros
#define PRIMAL_INTERNAL_TRACE(...)		Log::getCoreLogger()->trace(__VA_ARGS__)
#define PRIMAL_INTERNAL_INFO(...)		Log::getCoreLogger()->info(__VA_ARGS__)
#define PRIMAL_INTERNAL_WARN(...)		Log::getCoreLogger()->warn(__VA_ARGS__)
#define PRIMAL_INTERNAL_ERROR(...)		Log::getCoreLogger()->error(__VA_ARGS__)
#define PRIMAL_INTERNAL_CRITICAL(...)	Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define PRIMAL_TRACE(...)				Log::getClientLogger()->trace(__VA_ARGS__)
#define PRIMAL_INFO(...)				Log::getClientLogger()->info(__VA_ARGS__)
#define PRIMAL_WARN(...)				Log::getClientLogger()->warn(__VA_ARGS__)
#define PRIMAL_ERROR(...)				Log::getClientLogger()->error(__VA_ARGS__)
#define PRIMAL_CRITICAL(...)			Log::getClientLogger()->critical(__VA_ARGS__)

#pragma warning(pop)

#endif // log_h__