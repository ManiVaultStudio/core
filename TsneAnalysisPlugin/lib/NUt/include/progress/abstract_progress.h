#ifndef ABSTRACT_PROGRESS_H
#define ABSTRACT_PROGRESS_H

namespace nut{

	//! Abstract interface for a general progress
	class AbstractProgress{
	public:
	    virtual ~AbstractProgress(){}
		//! make a step forward
	    virtual void MakeAStep()=0;
		//! make N steps forward
	    virtual void MakeNSteps(unsigned int n)=0;
		//! test if the execution is canceled
	    virtual bool IsCanceled()const=0;
	};

	//! Abstract interface for a general progress
	class AbstractProgressUI: public AbstractProgress{
	public:
	    virtual ~AbstractProgressUI(){}
	    //! reset the progress
		virtual void Reset() =0;
		//! set the maximum value on the UI
		virtual void SetMaximum(int maximum) =0;
		//! get the maximum value on the UI
		virtual int GetMaximum() const =0;
		//! set the minimum value on the UI
		virtual void SetMinimum(int minimum) =0;
		//! get the minimum value on the UI
		virtual int GetMinimum() const =0; 
		//! set the current position on the UI
		virtual void SetCurrentPosition(int pos) =0;
		//! get the current position on the UI
		virtual int GetCurrentPosition() const =0;
	};

}
#endif // ABSTRACT_PROGRESS_H
