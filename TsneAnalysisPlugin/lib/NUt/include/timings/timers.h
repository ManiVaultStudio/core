#ifndef TIMERS_H
#define TIMERS_H

#include <chrono>
#include <ctime>
#include "helpers/initialization_helper.h"
#include "timings/utils.h"

namespace nut{

	class Timer{
	private:
		typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

	public:
		//! return the elapsed time
		template <class UnitOfMeas>
		double ElapsedTime()const;
		//! start the timer
		void Start();
		//! stop the timer
		void Stop();
		//! return the timer condition
		bool IsStarted()const;
		//! return the availability of the elapsed time
		bool IsElapsedTimeAvailable()const;

	private:
		TimePoint start_;
		TimePoint stop_;

		InitializationHelper startedIHelper_;
		InitializationHelper timeAvailableIHelper_;
	};

	
}

//Implementation
#include "timers-inl.h"

#endif // TIMERS_H
