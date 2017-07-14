#ifndef COUT_PROGRESS_H
#define COUT_PROGRESS_H

#include "progress/abstract_progress.h"
#include "log/cout_log.h"

namespace nut{

	//! A progress class which prints something on the std::cout
	class CoutProgress: public AbstractProgressUI{
	public:
		CoutProgress():minimum_(0),maximum_(100),current_position_(0),progress_bar_tokens_(30){}
	    virtual ~CoutProgress(){}
	    //! make a step forward
	    virtual void MakeAStep();
		//! make N steps forward
	    virtual void MakeNSteps(unsigned int steps);
		//! test if the execution is canceled
	    virtual bool IsCanceled()const;
	    //! reset the progress
		virtual void Reset();
		//! set the maximum value on the UI
		virtual void SetMaximum(int maximum);
		//! get the maximum value on the UI
		virtual int GetMaximum() const;
		//! set the minimum value on the UI
		virtual void SetMinimum(int minimum);
		//! get the minimum value on the UI
		virtual int GetMinimum() const; 
		//! set the current position on the UI
		virtual void SetCurrentPosition(int pos);
		//! get the current position on the UI
		virtual int GetCurrentPosition() const;
		//! get the number of tokens in the progress bar
		void progress_bar_tokens(int progress_bar_tokens);
		//! set the number of tokens in the progress bar
		int get_progress_bar_tokens();

	private:
		int minimum_;
		int maximum_;
		int current_position_;
		int progress_bar_tokens_;
		CoutLog log_;
	};

}

#include "cout_progress-inl.h"

#endif // ABSTRACT_PROGRESS_H
