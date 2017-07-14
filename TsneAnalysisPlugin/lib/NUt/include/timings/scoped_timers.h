#ifndef SCOPED_TIMERS_H
#define SCOPED_TIMERS_H

#include <chrono>
#include <ctime>
#include "timings/timers.h"
#include "timings/utils.h"

namespace nut{

	template <class T, class UnitOfMeas = Milliseconds>
	class ScopedTimer{
	public:
		//! start the timer
		ScopedTimer(T& elapsed_time);
		//! stop the timer and save the elapsedTime
		~ScopedTimer();
	
	private:
		Timer 	timer_;
		T& 		elapsed_time_;
	};

	template <class T, class UnitOfMeas = Milliseconds>
	class ScopedIncrementalTimer{
	public:
		//! start the timer
		ScopedIncrementalTimer(T& elapsed_time);
		//! stop the timer and save the elapsedTime
		~ScopedIncrementalTimer();
	
	private:
		Timer 	timer_;
		T& 		elapsed_time_;
	};
	
}

//Implementation
#include "scoped_timers-inl.h"

#endif // SCOPED_TIMERS_H
