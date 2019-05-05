#ifndef timer_h__
#define timer_h__

#include <chrono>

class Timer
{
	public:
		Timer()
		{
			reset();
		}

		void reset()
		{
			mStart = HighResolutionClock::now();
		}

		float elapsed() const
		{
			return std::chrono::duration_cast<MilliSecondsType>(HighResolutionClock::now() - mStart).count() / 1000.0f;
		}

		float elapsedMillis() const
		{
			return elapsed() * 1000.0f;
		}

	private:
		using HighResolutionClock = std::chrono::high_resolution_clock;
		using MilliSecondsType = std::chrono::duration<float, std::milli>;

		std::chrono::time_point<HighResolutionClock> mStart;
};

#endif // timer_h__