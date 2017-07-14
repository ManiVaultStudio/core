#ifndef GRADIENT_DESCENT_H
#define GRADIENT_DESCENT_H

#include "traits.h"
#include "log/abstract_log.h"
#include "helpers/initialization_helper.h"

namespace atsne{

	template <class Traits = TSNEDefaultTraits>
	class GradientDescent{
	public:
		typedef typename Traits::scalar_type scalar_type;
		typedef typename Traits::vector_type vector_type;
		typedef typename Traits::flag_type flag_type;
		typedef typename Traits::sparse_matrix_type sparse_matrix_type;

		class Params{
		public:
			Params():
				_random_seed(-1),
				_theta(0.5),//0.5
				_mom_switching_iter(250),//250
				_stop_lying_iter(250),//250
				_momentum(0.5),//0.5
				_final_momentum(0.8),//0.8
				_eta(200.),//200.
				_minimum_gain(0.01),//0.01
				_u_mult_for_gradient_control(7.5),
                _exponential_decay(0.95),
                _compute_error_at_each_iteration(false),
				_use_exponential_decay_as_early_exaggeration_removal(false)
			{}
		public:
			int _random_seed;
			double	_theta;
			int		_mom_switching_iter;
			int		_stop_lying_iter;
			double	_momentum;
			double	_final_momentum;
			double	_eta;
			double	_minimum_gain;
			double  _u_mult_for_gradient_control;
			double	_exponential_decay;

			bool	_compute_error_at_each_iteration;
			bool	_use_exponential_decay_as_early_exaggeration_removal;
		};

	public:
		GradientDescent();
		void Initialize(){}
		void Initialize(const sparse_matrix_type* sparse_matrix, flag_type* flags, scalar_type* data, scalar_type* solution, int desired_dimensions);
		void DoAnIteration();
		void Reset();
		int GetCurrentIteration()const{return _iter;}

		const vector_type& min_per_dimension()const{ return _min_per_dimension; }
		const vector_type& max_per_dimension()const{ return _max_per_dimension; }

		scalar_type total_error()const{return _total_error;}
		scalar_type min_error()const{return _min_error;}
		scalar_type max_error()const{return _max_error;}

	private:
		static inline scalar_type RandInit();
		static inline double sign(double x) { return (x == .0 ? .0 : (x < .0 ? -1.0 : 1.0)); }

	private:
		nut::InitializationHelper _initialization_helper;

		const sparse_matrix_type* _sparse_matrix;
		flag_type* _flags;
		int _desired_dimensions;
		int _num_points;

		scalar_type* _solution;
		vector_type _dY;
		vector_type _uY;
	
		vector_type	_errors;
		scalar_type	_min_error;
		scalar_type	_max_error;
		scalar_type	_total_error;

		vector_type _min_per_dimension;
		vector_type _max_per_dimension;

		int _iter;

		vector_type _gains;

	public:
		mutable nut::AbstractLog* _log;
        bool   _verbose;
		Params _param;
	};

}

#include "gradient_descent-inl.h"

#endif
