#ifndef GRADIENT_DESCENT_INL_H
#define GRADIENT_DESCENT_INL_H

#include "log/log_helper_functions.h"
#include "utils.h"
#include "tsne_errors.h"
#include "constants.h"

#ifdef __USE_GCD__
#include <dispatch/dispatch.h>
#else
#include "omp.h"
#endif

namespace atsne{

	template <class Traits>
	GradientDescent<Traits>::GradientDescent() :
    _sparse_matrix(nullptr),
    _log(nullptr)
	{

	}

	template <class Traits>
	void GradientDescent<Traits>::Initialize(const sparse_matrix_type* sparse_matrix, flag_type* flags, scalar_type* data, scalar_type* solution, int desired_dimensions){
		nut::SecureLog(_log,"Initialization of the t-SNE gradient descent");
		_initialization_helper.Initialize();

		_iter = 0;
		_desired_dimensions = desired_dimensions;
		_num_points = sparse_matrix->NumPoints();
		_flags = flags;
		_sparse_matrix = sparse_matrix;
		_solution = solution;

		_errors.resize(_num_points,0);
		_dY.	resize(_num_points*desired_dimensions,0);
		_uY.	resize(_num_points*desired_dimensions,0);
		_gains.	resize(_num_points*desired_dimensions,1.0);

		if(_param._random_seed == -1){
			std::srand(time(NULL));
		}else{
			std::srand(_param._random_seed);
		}
		for(int i = 0; i < _num_points*_desired_dimensions; ++i){
			_solution[i] = RandInit() * .0001;
		}

		_min_per_dimension.clear();
		_max_per_dimension.clear();

		_min_per_dimension.resize(desired_dimensions,-1);
		_max_per_dimension.resize(desired_dimensions,1);

		double exaggeration_factor = 10 + _num_points/5000.;
        nut::SecureLogValue(_log, "Exaggeration factor", exaggeration_factor, _verbose);

		for(scalar_type& exaggeration : _sparse_matrix->_exaggeration_factors){
			exaggeration = exaggeration_factor;
		}
	}

	template <class Traits>
	void GradientDescent<Traits>::DoAnIteration(){
		_initialization_helper.CheckInitialized();

		SPTree tree(_desired_dimensions, _solution, _num_points);
    
		double sum_Q = .0;
        std::vector<double> pos_f(_num_points*_desired_dimensions);
//#ifndef __USE_GCD__
        std::vector<double> neg_f(_num_points*_desired_dimensions);
//#else
//        __block	std::vector<double> neg_f(_num_points*_desired_dimensions);
//#endif

		if(!_param._use_exponential_decay_as_early_exaggeration_removal){
			//No exponential decay
			if(_iter == _param._stop_lying_iter){
				for(scalar_type& exaggeration : _sparse_matrix->_exaggeration_factors){
					exaggeration = 1.;
				}
			}else if(_iter > _param._stop_lying_iter){
				//Exponential decay after the _stop_lying_iter
				for(scalar_type& exaggeration : _sparse_matrix->_exaggeration_factors){
					exaggeration *= _param._exponential_decay;
					if(exaggeration < 1){
						exaggeration = 1.;
					}
				}
			}
		}else{
			//Exponential decay
			if(_iter >= _param._stop_lying_iter){
				for(scalar_type& exaggeration : _sparse_matrix->_exaggeration_factors){
					exaggeration *= _param._exponential_decay;
					if(exaggeration < 1){
						exaggeration = 1.;
					}
				}
			}
		}

		tree.computeEdgeForces(*_sparse_matrix, pos_f.data());
        
//#ifndef __USE_GCD__
        std::vector<double> sum_Q_subvalues(_num_points,0);
        #pragma omp parallel for num_threads(omp_get_max_threads()-1)
        for(int n = 0; n < _num_points; n++){
//#else
//        __block std::vector<double> sum_Q_subvalues(_num_points,0);
//        dispatch_apply(_num_points, dispatch_get_global_queue(0, 0), ^(size_t n) {
//#endif //__USE_GCD__
			tree.computeNonEdgeForcesOMP(n, _param._theta, neg_f.data() + n * _desired_dimensions, sum_Q_subvalues[n]);
        }
//#ifdef __USE_GCD__
//        );
//#endif

		sum_Q = 0;
		for(int n = 0; n < _num_points; n++){
			sum_Q += sum_Q_subvalues[n];
		}

		//error at iteration iter-1
		if(_param._compute_error_at_each_iteration){
			_total_error = 0;
			_max_error = 0;
			_min_error = 0;
			TSNEErrorUtils<Traits>::ComputeBarnesHutTSNEError(*_sparse_matrix,_solution,_desired_dimensions,sum_Q,_total_error,_min_error,_max_error);
		}
    
		for(scalar_type& dimension : _min_per_dimension){
			dimension = std::numeric_limits<scalar_type>::max();
		}
		for(scalar_type& dimension : _max_per_dimension){
			dimension = std::numeric_limits<scalar_type>::min();
		}

		// Update _gains
		// Perform gradient update (with momentum and _gains)
		for(int i = 0; i < _num_points * _desired_dimensions; i++){
			// Compute final t-SNE gradient
			_dY[i] = pos_f[i] - (neg_f[i] / sum_Q);
		
			const int point_idx = i / _desired_dimensions;
			const int dimension_idx = i - (point_idx * _desired_dimensions);
			if((_flags[point_idx] & constants::Flag::Fixed) == constants::Flag::Fixed){
				continue;
			}
			_gains[i] = (sign(_dY[i]) != sign(_uY[i])) ? (_gains[i] + .2) : (_gains[i] * .8);

			if(_gains[i] < _param._minimum_gain){
				_gains[i] = _param._minimum_gain;
			}
			if(_uY[i] != 0){
				_dY[i] = (_dY[i]>0?1:-1)*std::min(std::abs(_dY[i]*_param._eta* _gains[i]),std::abs(_uY[i]*_param._u_mult_for_gradient_control))/(_param._eta*_gains[i]);
			}
			//_dY[i] = (_dY[i]>0?1:-1)*std::abs(_dY[i]*_param._eta* _gains[i]);

			_uY[i] = ((_iter<_param._mom_switching_iter)?_param._momentum:_param._final_momentum) * _uY[i] - _param._eta * _gains[i] * _dY[i];
			_solution[i] += _uY[i];

			if(_solution[i] > _max_per_dimension[dimension_idx]){
				_max_per_dimension[dimension_idx] = _solution[i];
			}
			if(_solution[i] < _min_per_dimension[dimension_idx]){
				_min_per_dimension[dimension_idx] = _solution[i];
			}
		}
        
		// Make solution zero-mean
		Utils<Traits>::ZeroMean(_solution, _num_points, _desired_dimensions);
        
		if(_iter == _param._mom_switching_iter) {
            nut::SecureLog(_log, "Switch momentum!",_verbose);
		}
		++_iter;

	
	}

	template <class Traits>
	void GradientDescent<Traits>::Reset(){
		nut::SecureLog(_log,"Resetting of the t-SNE gradient descent");
		_initialization_helper.Uninitialize();

		_iter = 0;
		_desired_dimensions = 0;
		_flags = nullptr;
		_sparse_matrix = nullptr;
		_solution = nullptr;

		_errors.clear();
		_dY.	clear();
		_uY.	clear();
		_gains.	clear();
	}

	template <class Traits>
	typename GradientDescent<Traits>::scalar_type GradientDescent<Traits>::RandInit() {
		double x, y, radius;
		do {
			x = 2 * (rand() / ((double) RAND_MAX + 1)) - 1;
			y = 2 * (rand() / ((double) RAND_MAX + 1)) - 1;
			radius = (x * x) + (y * y);
		} while((radius >= 1.0) || (radius == 0.0));
		radius = sqrt(-2 * log(radius) / radius);
		x *= radius;
		y *= radius;
		return x;
	}
}

#endif
