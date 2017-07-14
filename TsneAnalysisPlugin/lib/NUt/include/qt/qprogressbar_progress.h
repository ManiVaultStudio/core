#ifndef QPROGRESSBAR_PROGRESS_H
#define QPROGRESSBAR_PROGRESS_H

#include <progress/abstract_progress.h>
#include <helpers/initialization_helper.h>
#include <QProgressBar>

namespace nut{

	//! display a generic progress witha a QProgressBar
	class QProgressBarProgress: public AbstractProgress{
	public:
		QProgressBarProgress(QProgressBar* progress_bar = nullptr);
	    virtual ~QProgressBarProgress(){}
	    //! reset the progress
		void Reset();
		//! set the maximum value on the UI
		void SetMaximum(int maximum);
		//! get the maximum value on the UI
		int GetMaximum() const;
		//! set the minimum value on the UI
		void SetMinimum(int minimum);
		//! get the minimum value on the UI
		int GetMinimum() const; 
		//! set the current position on the UI
		void SetCurrentPosition(int pos);
		//! get the current position on the UI
		int GetCurrentPosition() const;
		//! make a step forward
		void MakeAStep();
		//! make N steps forward
		void MakeNSteps(unsigned int n);
		//! test if the execution is canceled
	    bool IsCanceled()const;
	public:
		QProgressBar* _progress_bar;
	};

		//! display a generic progress witha a QProgressBar
	class AsyncQProgressBarProgress: public QObject, public AbstractProgress{
		Q_OBJECT
	public:
		AsyncQProgressBarProgress();
	    virtual ~AsyncQProgressBarProgress(){}
	    //! reset the progress
		void Reset();
		//! set the maximum value on the UI
		void SetMaximum(int maximum);
		//! get the maximum value on the UI
		int GetMaximum() const;
		//! set the minimum value on the UI
		void SetMinimum(int minimum);
		//! get the minimum value on the UI
		int GetMinimum() const; 
		//! set the current position on the UI
		void SetCurrentPosition(int pos);
		//! get the current position on the UI
		int GetCurrentPosition() const;
		//! make a step forward
		void MakeAStep();
		//! make N steps forward
		void MakeNSteps(unsigned int n);
		//! test if the execution is canceled
	    bool IsCanceled()const;
		//! initialize the async qprogressbar progress
		void Initialize(QProgressBar* ptr);
		//! uninitialize the async qprogressbar progress
		void Uninitialize();

	Q_SIGNALS:
		void asyncReset();
		void asyncSetMaximum(int maximum);
		void asyncSetMinimum(int minimum);
		void asyncSetValue(int value);

	private:
		QProgressBar*			_progress_bar;
		InitializationHelper	_initialization_helper;
	};
	
}

//Implementation
#include <qt/qprogressbar_progress-inl.h>

#endif
