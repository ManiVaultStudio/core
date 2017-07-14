#ifndef WORKER_QTHREAD_H
#define WORKER_QTHREAD_H

#include <QThread>
#include <log/abstract_log.h>
#include <progress/progress_reduction.h>

namespace nut{

	template <class Derived>
	class WorkerQThread: public QThread{
	public:
		WorkerQThread();
	private:
		void PreProcess();
		void Process();
		void PostProcess();
		void run();
	public:
		AbstractLog*		_log;
		ProgressReduction*	_prg;
		bool				_verbose;
		std::string			_thread_name;
	};
	
}

//Implementation
#include <qt/worker_qthread-inl.h>

#endif 
