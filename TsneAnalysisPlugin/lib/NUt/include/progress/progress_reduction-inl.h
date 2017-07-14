#ifndef PROGRESS_REDUCTION_INL
#define PROGRESS_REDUCTION_INL

#if defined(_OPENMP)
	#include "omp.h"
#endif

#include "errors/invariants.h"
namespace nut{
	
	inline ProgressReduction::ProgressReduction(AbstractProgress* external_progress, unsigned int num_external_steps):
		external_progress_(external_progress),
		num_external_steps_(num_external_steps),
		num_internal_steps_(0),
		reduction_ratio_(0),
		last_emitted_step_(0),
		internal_steps_(0)
	{
	}
	inline void ProgressReduction::MakeAStep(){
		if(external_progress_ == nullptr)
			return;
#if defined(_OPENMP)
		#pragma omp critical
#endif
		{
			internal_steps_ += reduction_ratio_;
		}
		
#if defined(_OPENMP)
		if(omp_get_thread_num() != 0){
			return;
		}
#endif
		const double to_increment(internal_steps_ - last_emitted_step_);

		assert(to_increment >= 0);

		unsigned int to_increment_int (to_increment);
		external_progress_->MakeNSteps(to_increment_int);
		last_emitted_step_ += to_increment_int;
	}
	inline void ProgressReduction::MakeNSteps(unsigned int n){
		for(unsigned int i = 0; i < n; ++i){
			MakeAStep();
		}
	}

	inline bool ProgressReduction::IsCanceled()const{
		return (external_progress_ != nullptr)?external_progress_->IsCanceled():false;
	}
	inline void ProgressReduction::Reset(){
		last_emitted_step_ = 0;
		num_internal_steps_ = 0;
		internal_steps_ = 0.;
	}
	inline void ProgressReduction::Finalize(){
		PRECONDITION(last_emitted_step_ <= num_external_steps_);
		if(external_progress_ == nullptr) 
			return;
		int final_steps (static_cast<int>(num_external_steps_-last_emitted_step_));
		assert(final_steps >= 0);
		for(int i = 0; i < final_steps; ++i){
			external_progress_->MakeAStep();
			++last_emitted_step_;
		}
		internal_steps_ = last_emitted_step_;
	}

	inline void ProgressReduction::set_num_external_steps(unsigned int steps){
		num_external_steps_ = steps;
		reduction_ratio_ =(num_internal_steps_!=0)?double(num_external_steps_)/num_internal_steps_:0;
	}
	inline void ProgressReduction::set_num_internal_steps(unsigned int steps){
		num_internal_steps_ = steps;
		reduction_ratio_ =(num_internal_steps_!=0)?double(num_external_steps_)/num_internal_steps_:0;
	}
	inline void ProgressReduction::set_external_progress(AbstractProgress* prg){
		external_progress_ = prg;
	}


	inline unsigned int ProgressReduction::num_external_steps()const{
		return num_external_steps_;
	}
	inline unsigned int ProgressReduction::num_internal_steps()const{
		return num_internal_steps_;
	}
	inline double ProgressReduction::reduction_ratio()const{
		return reduction_ratio_;
	}

	inline AbstractProgress* ProgressReduction::external_progress(){
		return external_progress_;
	}
	inline const AbstractProgress* ProgressReduction::external_progress()const{
		return external_progress_;
	}

}

#endif // PROGRESS_REDUCTION_INL