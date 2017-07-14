/*
 *
 * Copyright (c) 2014, Nicola Pezzotti (Delft University of Technology)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *    This product includes software developed by the Delft University of Technology.
 * 4. Neither the name of the Delft University of Technology nor the names of
 *    its contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY LAURENS VAN DER MAATEN ''AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL LAURENS VAN DER MAATEN BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 */

#ifndef DENSITY_BASED_REFINEMENT_STRATEGY_H
#define DENSITY_BASED_REFINEMENT_STRATEGY_H

#include "refinement_strategy.h"
#include "approx_tsne.h"
#include <mutex>
#include <ctime>
#include <thread>
#include <algorithm>
#include <numeric>
#include "log/log_helper_functions.h"

template <class Traits = TSNEDefaultTraits>
class DensityBasedRefinementStrategy: public RefinementStrategy<Traits>{
public:
	typedef typename Traits::scalar_type scalar_type;
	typedef typename Traits::vector_type vector_type;
	typedef typename Traits::flag_type flag_type;
	typedef typename Traits::sparse_matrix_type sparse_matrix_type;

public:
	DensityBasedRefinementStrategy():_active(false),_idx(0){}

	virtual void Initialize();
	virtual void Refine();
	virtual void StopRefinement();
	virtual bool IsActive(){return _active;}

	virtual std::time_t GetStartTime(){return 0;}
	virtual std::time_t GetEstimatedFinishTime(){return 0;}

private:
	void InitializationFunction();
	void RefinementFunction();

private:
	std::thread _initialization_thread;
	std::thread _refinement_thread;
	TSNEExactRefiner<> _exact_refiner;
	TSNEFlannRefiner<> _approx_refiner;

	std::vector<std::pair<scalar_type,int>> _density_based_order;
	bool _active;
	int _idx;
};


template <class Traits>
void DensityBasedRefinementStrategy<Traits>::Initialize(){
	_initialization_thread = std::thread(&DensityBasedRefinementStrategy<Traits>::InitializationFunction,this);
}


template <class Traits>
void DensityBasedRefinementStrategy<Traits>::Refine(){
	_active = true;
	_refinement_thread = std::thread(&DensityBasedRefinementStrategy<Traits>::RefinementFunction,this);
}

template <class Traits>
void DensityBasedRefinementStrategy<Traits>::StopRefinement(){
	_active = false; 
	if(_refinement_thread.joinable())
		_refinement_thread.join();
}

template <class Traits>
void DensityBasedRefinementStrategy<Traits>::InitializationFunction(){
	nut::SecureLog(this->_log,"Initializing density based refinement strategy...");
	assert(this->_param._perplexity > 3);
	bool exact(this->_param._desired_precision >= 1.);

	//initialization of the indicies
	_density_based_order.reserve(this->_sparse_matrix_ptr->NumPoints());
	for(int i = 0; i < this->_sparse_matrix_ptr->NumPoints(); ++i){
		_density_based_order.push_back(std::make_pair(1/this->_sparse_matrix_ptr->_beta[i],i));
	}
	std::sort(_density_based_order.begin(),_density_based_order.end());

	//initialization of the KNN data structure
	if(exact){
		_exact_refiner._param._perplexity = this->_param._perplexity;
		_exact_refiner._param._perplexity_multiplier = this->_param._perplexity_mult;
		_exact_refiner.Initialize(this->_param._data,this->_param._num_data,this->_param._num_dimensions);
		nut::SecureLog(this->_log,"density based refinement strategy initialization complete");
	}else{
		_approx_refiner._param._perplexity = this->_param._perplexity;
		_approx_refiner._param._perplexity_multiplier = this->_param._perplexity_mult;
		_approx_refiner._param._precision = this->_param._desired_precision;
		_approx_refiner.Initialize(this->_param._data,this->_param._num_data,this->_param._num_dimensions);
		nut::SecureLog(this->_log,"density based refinement strategy initialization complete");
	}
}

template <class Traits>
void DensityBasedRefinementStrategy<Traits>::RefinementFunction(){
	if(_initialization_thread.joinable())
		_initialization_thread.join();

	bool exact(this->_param._desired_precision >= 1.);
	
	if(exact){
		int i;
		for(i = 0; i < this->_param._num_data && _active; ++i){
			_exact_refiner.RefineHighDimensionalProbDistributions(_density_based_order[i].second,*this->_sparse_matrix_ptr);
			if((i%1000) == 0){
				nut::SecureLogValue(this->_log,"Exact refinement iteration: ",i);
			}
		}
	}else{
		int i;
		for(i = 0; i < this->_param._num_data && _active; ++i){
			_approx_refiner.RefineHighDimensionalProbDistributions(_density_based_order[i].second,*this->_sparse_matrix_ptr);
			if((i%1000) == 0){
				nut::SecureLogValue(this->_log,"Approximated refinement iteration: ",i);
			}
		}
	}
}


#endif
