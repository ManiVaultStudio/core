#ifndef WORKER_QTHREAD_INL
#define WORKER_QTHREAD_INL

#include <log/log_helper_functions.h>

namespace nut{

	template <class Derived>
	WorkerQThread<Derived>::WorkerQThread():
		_log(nullptr),
		_prg(nullptr),
		_verbose(false),
		_thread_name("Unnamed-Worker-Thread")
	{
	
	}
	template <class Derived>
	void WorkerQThread<Derived>::PreProcess(){
		SecureLogValue(_log,"A Worker thread is entering in the preprocessing stage",_thread_name,_verbose);
		static_cast<Derived*>(this)->PreProcessImpl();
	}
	template <class Derived>
	void WorkerQThread<Derived>::Process(){
		SecureLogValue(_log,"A Worker thread is entering in the processing stage",_thread_name,_verbose);
		static_cast<Derived*>(this)->ProcessImpl();	
	}
	template <class Derived>
	void WorkerQThread<Derived>::PostProcess(){
		SecureLogValue(_log,"A Worker thread is entering in the postprocessing stage",_thread_name,_verbose);
		static_cast<Derived*>(this)->PostProcessImpl();
	}
	template <class Derived>
	void WorkerQThread<Derived>::run(){
		SecureLogValue(_log,"A Worker thread has been launched",_thread_name,_verbose);
		PreProcess();
		Process();
		PostProcess();
		SecureLogValue(_log,"A Worker thread has completed its execution",_thread_name,_verbose);
	}
}


#endif 
