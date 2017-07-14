#ifndef SCOPED_TIMERS_INL
#define SCOPED_TIMERS_INL

#include <chrono>
#include <ctime>
#include "timings/timers.h"
#include "timings/utils.h"

namespace nut{

	template <class T, class UnitOfMeas>
	ScopedTimer<T,UnitOfMeas>::ScopedTimer(T& elapsed_time):
		elapsed_time_(elapsed_time)
	{
		timer_.Start();
	}

	template <class T, class UnitOfMeas>
	ScopedTimer<T,UnitOfMeas>::~ScopedTimer(){
		timer_.Stop();
		elapsed_time_ = static_cast<T>(timer_.ElapsedTime<UnitOfMeas>());
	}
	

	template <class T, class UnitOfMeas>
	ScopedIncrementalTimer<T,UnitOfMeas>::ScopedIncrementalTimer(T& elapsed_time):
		elapsed_time_(elapsed_time)
	{
		timer_.Start();
	}

	template <class T, class UnitOfMeas>
	ScopedIncrementalTimer<T,UnitOfMeas>::~ScopedIncrementalTimer(){
		timer_.Stop();
		elapsed_time_ += static_cast<T>(timer_.ElapsedTime<UnitOfMeas>());
	}
	
}

#endif // SCOPED_TIMERS_INL
