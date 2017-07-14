#ifndef TIMING_UTILS_H
#define TIMING_UTILS_H

#include <chrono>
#include <ctime>
#include <thread>

namespace nut{

	typedef std::chrono::milliseconds 	Milliseconds;
	typedef std::chrono::seconds 		Seconds;
	typedef std::chrono::minutes 		Minutes;
	typedef std::chrono::hours 			Hours;

	template <class UnitOfMeas>
	inline void SleepFor(unsigned int t){
		std::this_thread::sleep_for(UnitOfMeas(t));
	}
	
}

#endif // TIMING_UTILS_H
