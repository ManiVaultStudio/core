#ifndef PROGRESS_REDUCTION_H
#define PROGRESS_REDUCTION_H

#include "progress/abstract_progress.h"

namespace nut{

	
	class ProgressReduction: public AbstractProgress{
	public:
		ProgressReduction(AbstractProgress* external_progress = nullptr, unsigned int num_external_steps = 0);
	    virtual ~ProgressReduction(){}
	    //! make a step forward
	    virtual void MakeAStep();
		//! make N steps forward
		void MakeNSteps(unsigned int n);
		//! test if the execution is canceled
	    virtual bool IsCanceled()const;
	    //! reset the class
	    void Reset();
	    //! make all the remaining steps
		void Finalize();

		void set_num_external_steps(unsigned int steps);
		void set_num_internal_steps(unsigned int steps);
		void set_external_progress(AbstractProgress* prg);

		unsigned int num_external_steps()const;
		unsigned int num_internal_steps()const;
		double reduction_ratio()const;
		AbstractProgress* external_progress();
		const AbstractProgress* external_progress()const;

	private:
		unsigned int num_external_steps_;
		unsigned int num_internal_steps_;

		unsigned int last_emitted_step_;
		double internal_steps_;
		double reduction_ratio_;

		AbstractProgress* external_progress_;
	};

	inline void SecureMakeAStep(ProgressReduction* prg){
		if(prg!=nullptr){
			prg->MakeAStep();
		}
	}
	inline void SecureFinalize(ProgressReduction* prg){
		if(prg!=nullptr){
			prg->Finalize();
		}
	}
	inline void SecureSetInternalSteps(ProgressReduction* prg, unsigned int steps){
		if(prg!=nullptr){
			prg->set_num_internal_steps(steps);
		}
	}
	inline void SecureSetExternalSteps(ProgressReduction* prg, unsigned int steps){
		if(prg!=nullptr){
			prg->set_num_external_steps(steps);
		}
	}

}

#include "progress_reduction-inl.h"

#endif // PROGRESS_REDUCTION_H
