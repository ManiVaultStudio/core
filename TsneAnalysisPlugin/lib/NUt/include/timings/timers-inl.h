#ifndef TIMERS_INL
#define TIMERS_INL

namespace nut{

	template <class UnitOfMeas>
	double Timer::ElapsedTime()const{
		timeAvailableIHelper_.CheckInitialized();
		return std::chrono::duration<double,typename UnitOfMeas::period>(stop_-start_).count();
	}
	inline void Timer::Start(){
		startedIHelper_.CheckUninitialized();
		startedIHelper_.Initialize();
		if(timeAvailableIHelper_.IsInitialized())
			timeAvailableIHelper_.Uninitialize();
		start_ = std::chrono::system_clock::now();
	}
	inline void Timer::Stop(){
		startedIHelper_.CheckInitialized();
		startedIHelper_.Uninitialize();
		timeAvailableIHelper_.Initialize();
		stop_ = std::chrono::system_clock::now();
	}
	inline bool Timer::IsStarted()const{
		return startedIHelper_.IsInitialized();
	}
	inline bool Timer::IsElapsedTimeAvailable()const{
		return timeAvailableIHelper_.IsInitialized();
	}
	
}

#endif // TIMERS_INL