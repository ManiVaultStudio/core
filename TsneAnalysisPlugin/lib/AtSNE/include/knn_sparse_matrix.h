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

#ifndef KNN_SPARSE_MATRIX_H
#define KNN_SPARSE_MATRIX_H

#include <mutex>
#include <unordered_map>
#include <map>
#include <vector>

template <class T>
class KNNSparseMatrix{
public:
	typedef std::map<int,T> map_type;

public:
	KNNSparseMatrix(int N = 0);

	void ComputeNormalizationFactor();
	int NumPoints()const{return _N;}
	void Clear();
	void Resize(int N);

	void ExageratePointNeighborhood(int n, T factor/*, int rings = 1*/);

public:
	int _N;
	std::vector<map_type>	_knn_queries;
	std::vector<T>			_row_normalization;
	std::vector<T>			_beta;
	std::vector<T>			_knn_precision;
	mutable std::vector<T>	_exaggeration_factors;
	std::vector<std::map<int,std::tuple<std::tuple<int,T>,std::tuple<int, T> > > >	_symmetric_matrix; //two tuple inside may be an overkill... we'll see... refactor later
	double _normalization_factor; //_N*2

	//mutable std::mutex _write_mutex;
};

template <class T>
bool AreEqual(const KNNSparseMatrix<T>& a, const KNNSparseMatrix<T>& b, T eps);

template <class T>
void InitializeMatrixWithKNNQueries(const std::vector<std::map<int,double> >& knn_queries, KNNSparseMatrix<T>& matrix, double perplexity);

#include "knn_sparse_matrix-inl.h"

#endif
