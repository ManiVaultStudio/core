#ifndef COUT_PROGRESS_INL
#define COUT_PROGRESS_INL

#include "errors/invariants.h"
#include <string>
#include <sstream>

namespace nut{

    void CoutProgress::MakeAStep(){
    	if(current_position_ < maximum_)
    		current_position_ += 1;

    	const double percentage(double(current_position_-minimum_)/(maximum_-minimum_));
    	const int intbar_size(progress_bar_tokens_);
    	const int num_of_bar_tokens(static_cast<int>(percentage*intbar_size));
    	const int num_of_empty_tokens(intbar_size - num_of_bar_tokens);

    	std::stringstream ss;
    	ss << "Progress Bar |";
    	for(int i = 0; i < num_of_bar_tokens; ++i){
			ss << "=";
    	}
    	for(int i = 0; i < num_of_empty_tokens; ++i){
			ss << " ";
    	}
    	ss << "| " << percentage;
    	log_.Display(ss.str());
    }
	inline void CoutProgress::MakeNSteps(unsigned int n){
		for(unsigned int i = 0; i < n; ++i){
			MakeAStep();
		}
	}
    bool CoutProgress::IsCanceled()const{
		return false;
    }
	void CoutProgress::Reset(){
		current_position_ = minimum_;
	}
	void CoutProgress::SetMaximum(int maximum){
		maximum_ = maximum;
		if(current_position_ > maximum_)
			current_position_ = maximum_;
	}
	int CoutProgress::GetMaximum() const{
		return maximum_;
	}
	void CoutProgress::SetMinimum(int minimum){
		minimum_ = minimum;
		if(current_position_ < minimum_)
			current_position_ = minimum_;
	}
	int CoutProgress::GetMinimum() const{
		return minimum_;
	}
	void CoutProgress::SetCurrentPosition(int pos){
		PRECONDITION(pos >= minimum_ && pos <= maximum_);
		current_position_ = pos;
	}
	int CoutProgress::GetCurrentPosition() const{
		return current_position_;
	}
	void CoutProgress::progress_bar_tokens(int progress_bar_tokens){
		PRECONDITION(progress_bar_tokens > 0);
	}
	int CoutProgress::get_progress_bar_tokens(){
		return progress_bar_tokens_;
	}

}


#endif // ABSTRACT_PROGRESS_INL
