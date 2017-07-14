/*
 *
 * Copyright (c) 2014, Laurens van der Maaten (Delft University of Technology)
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

#ifndef TSNE_H
#define TSNE_H

#include <stdint.h>
#include <thread> 
#include "log/abstract_log.h"
#include "helpers/initialization_helper.h"
#include "approx_tsne.h"
#include "gradient_descent.h"

static inline double sign(double x) { return (x == .0 ? .0 : (x < .0 ? -1.0 : 1.0)); }


class TSNE
{    
public:
	typedef uint32_t flag_type;

public:
	class Param{
	public:
		Param():
			_perplexity(30),
			_theta(0.5),//0.5
			_mom_switching_iter(250),
			_stop_lying_iter(250),
			_momentum(0.5),//0.5
			_final_momentum(0.8),//0.8
			_eta(200.),//200.
			_minimum_gain(0.9),//0.01
			_compute_error_at_each_iteration(true)
		{}
	public:
		double	_perplexity;
		double	_theta;
		int		_mom_switching_iter;
		int		_stop_lying_iter;
		double	_momentum;
		double	_final_momentum;
		double	_eta;
		double	_minimum_gain;

		bool	_compute_error_at_each_iteration;
	};

public:
	TSNE();
	//~TSNE();

	void Init(double* X, flag_type* flags, int N, int D, double* Y, int no_dims, bool map_initialized=false);
	void DoAnIteration();
	void Reset();
	double GetCurrentError()const;
	int GetCurrentIteration()const;

	const std::vector<double>& GetErrors()const{return _errors;}
	const double& GetMinError()const{return _min_error;}
	const double& GetMaxError()const{return _max_error;}
	const double& GetTotalError()const{return _total_error;}

private:
	void computeGaussianPerplexity_Init(double* X, int N, int D, double* P, double perplexity);
    void computeGaussianPerplexity_Init(double* X, int N, int D, double perplexity, int K);
	
	void RefineGaussianPerplexity(double* X, int N, int D, double perplexity, int K);
    void symmetrizeMatrix_Init(int N);
	double randn_Init();


	void computeGradient_Iter(double* P, unsigned int* inp_row_P, unsigned int* inp_col_P, double* inp_val_P, double* Y, int N, int D, double* dC, double theta);
    void computeExactGradient_Iter(double* P, double* Y, int N, int D, double* dC);
    void computeSquaredEuclideanDistance_Iter(double* X, int N, int D, double* DD)const;
    void zeroMean_Iter(double* X, int N, int D);

    
	double evaluateError(const double* P, double* Y, int N, int D)const;
    double evaluateError(const unsigned int* row_P, const unsigned int* col_P, const double* val_P, double* Y, int N, int D, double theta)const;
    

private:
	nut::InitializationHelper _initialization_helper;

	std::vector<double> _dY;	//N*no_dims
	std::vector<double> _uY;	//N*no_dims
	std::vector<double> _gains;	//N*no_dims

	bool		_exact;
	double*		_X;
	flag_type*	_flags;
	int			_N; 
//	int			_D;
	double*		_Y; 
	int			_no_dims;
	bool		_map_initialized;

	std::vector<double>			_P;			// N * N
	std::vector<unsigned int>	_row_P;		// N + 1 
	std::vector<unsigned int>	_col_P;		// N * K
	std::vector<double>			_approx_P;	//val_P // N * K
	std::vector<double>			_approx_P_test;	//val_P // N * K

	std::vector<double>	_errors;
	double				_min_error;
	double				_max_error;
	double				_total_error;

//	double _sum_P;

	int _iter;

//	bool _to_destroy;
	std::thread _refinement_thread;

public:
	nut::AbstractLog*	_log;
	Param				_param;

//temp
	std::vector<double> _beta;
	std::vector<std::vector<double> > _big_matrix;

	KNNSparseMatrix<double> _sparse_matrix;
	atsne::GradientDescent<> _gradient_descent;
};

#endif
