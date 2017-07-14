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

#ifndef APPROX_TSNE_H
#define APPROX_TSNE_H

#include <stdint.h>
#include <vector> 
#include <thread> 
#include <unordered_map> 
#include "log/abstract_log.h"
#include "helpers/initialization_helper.h"
#include "vptree.h"
#include "knn_sparse_matrix.h"
#include <flann/flann.hpp>
#include "traits.h"



///////////////////////////////////////////////////////////////////                
////////////////////     Initialization     ///////////////////////                
///////////////////////////////////////////////////////////////////                  
template <class Traits = TSNEDefaultTraits>
class TSNEInitializer{
public:
	typedef typename Traits::scalar_type scalar_type;
	typedef typename Traits::vector_type vector_type;
	typedef typename Traits::flag_type flag_type;
	typedef typename Traits::sparse_matrix_type sparse_matrix_type;


	class Params{
	public:
		Params() :
			_perplexity(30),
			_perplexity_multiplier(3),
			_skip_normalization(false)
		{}
		int _perplexity;
		int _perplexity_multiplier;
		bool _skip_normalization;
	};

public:
	TSNEInitializer();
	void ComputeHighDimensionalProbDistributions(scalar_type* X, int N, int D, sparse_matrix_type& sparse_matrix)const;

public:
	mutable nut::AbstractLog* _log;
	Params _param;
};

template <class Traits = TSNEDefaultTraits>
class TSNEApproxInitializer{
public:
	typedef typename Traits::scalar_type scalar_type;
	typedef typename Traits::vector_type vector_type;
	typedef typename Traits::flag_type flag_type;
	typedef typename Traits::sparse_matrix_type sparse_matrix_type;


	class Params{
	public:
		Params() :
			_perplexity(30),
			_perplexity_multiplier(3),
			_num_checks(128),
			_num_trees(4),
			_cores(8),
			_skip_normalization(false)
		{}
		int _perplexity;
		int _perplexity_multiplier;
		int _num_checks;
		int _num_trees;
		int _cores;
		bool _skip_normalization;
	};

public:
	TSNEApproxInitializer();
	void ComputeHighDimensionalProbDistributions(scalar_type* X, int N, int D, sparse_matrix_type& sparse_matrix)const;

public:
	mutable nut::AbstractLog* _log;
    bool _verbose;
	Params _param;
};

///////////////////////////////////////////////////////////////////                
////////////////////////     Update     ///////////////////////////                
///////////////////////////////////////////////////////////////////    

template <class Traits = TSNEDefaultTraits>
class TSNEExactRefiner{
public:
	typedef typename Traits::scalar_type scalar_type;
	typedef typename Traits::vector_type vector_type;
	typedef typename Traits::flag_type flag_type;
	typedef typename Traits::sparse_matrix_type sparse_matrix_type;

private:
	typedef VpTree<DataPoint,euclidean_distance<double> > vantage_point_tree_type;
	typedef DataPoint tree_data_type;
public:

	class Params{
	public:
		Params():
			_perplexity(30),
			_perplexity_multiplier(3),
			_cores(1),
			_exaggeration(5)
		{}
		int _perplexity;
		int _perplexity_multiplier;
		int _cores;
		double _exaggeration;
	};

public:
	TSNEExactRefiner();
	void Initialize(const scalar_type* X, int N, int D);

	void RefineHighDimensionalProbDistributions(int n, sparse_matrix_type& sparse_matrix)const;

public:
	mutable nut::AbstractLog* _log;
	Params _param;

private:
	vantage_point_tree_type _tree;
	std::vector<DataPoint> _obj_X;
	scalar_type* _X;
	int _N;
	int _D;
};


template <class Traits = TSNEDefaultTraits>
class TSNEKdTreeForestRefiner{
public:
	typedef typename Traits::scalar_type scalar_type;
	typedef typename Traits::vector_type vector_type;
	typedef typename Traits::flag_type flag_type;
	typedef typename Traits::sparse_matrix_type sparse_matrix_type;

private:
	typedef flann::Matrix<int>		int_matrix_type;
	typedef flann::Matrix<double>	double_matrix_type;
	typedef std::vector<int>		int_vector_type;
	typedef std::vector<double>		double_vector_type;
public:

	class Params{
	public:
		Params() :
			_perplexity(30),
			_perplexity_multiplier(3),
			_num_checks(128),
			_num_trees(4),
			_cores(1),
			_exaggeration(5)
		{}
		int _perplexity;
		int _perplexity_multiplier;
		int _num_checks;
		int _num_trees;
		int _cores;
		double _exaggeration;
	};

public:
	TSNEKdTreeForestRefiner();
	void Initialize(scalar_type* X, int N, int D);

	void RefineHighDimensionalProbDistributions(int n, sparse_matrix_type& sparse_matrix)const;

public:
	mutable nut::AbstractLog* _log;
	Params _param;

private:
	int_vector_type		_indices;
	double_vector_type	_distances;
	scalar_type* _X;
	int _N;
	int _D;
};



template <class Traits = TSNEDefaultTraits>
class TSNEFlannRefiner{
public:
	typedef typename Traits::scalar_type scalar_type;
	typedef typename Traits::vector_type vector_type;
	typedef typename Traits::flag_type flag_type;
	typedef typename Traits::sparse_matrix_type sparse_matrix_type;

private:
	typedef flann::Matrix<int>		int_matrix_type;
	typedef flann::Matrix<double>	double_matrix_type;
	typedef std::vector<int>		int_vector_type;
	typedef std::vector<double>		double_vector_type;
public:

	class Params{
	public:
		Params() :
			_perplexity(30),
			_perplexity_multiplier(3),
			_precision(0.7),
			_cores(1),
			_exaggeration(5)
		{}
		int _perplexity;
		int _perplexity_multiplier;
		int _precision;
		int _cores;
		double _exaggeration;
	};

public:
	TSNEFlannRefiner();
	void Initialize(scalar_type* X, int N, int D);

	void RefineHighDimensionalProbDistributions(int n, sparse_matrix_type& sparse_matrix)const;

public:
	mutable nut::AbstractLog* _log;
	Params _param;

private:
	int_vector_type		_indices;
	double_vector_type	_distances;
	scalar_type* _X;
	int _N;
	int _D;
};

#endif
