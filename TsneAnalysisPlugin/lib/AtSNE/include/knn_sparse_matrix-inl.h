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

#ifndef KNN_SPARSE_MATRIX_INL
#define KNN_SPARSE_MATRIX_INL

#include <queue>
#include <cmath>

template <class T>
KNNSparseMatrix<T>::KNNSparseMatrix(int N) :
    _N(N),
    _knn_queries(N),
	_row_normalization(N,0.),
	_beta(N,0.),
	_knn_precision(N,0.),
    _exaggeration_factors(N,1),
    _symmetric_matrix(N),
    _normalization_factor(0.)
{
	
}

template <class T>
void KNNSparseMatrix<T>::ComputeNormalizationFactor(){
	_normalization_factor = _N*2;
}
template <class T>
void KNNSparseMatrix<T>::Clear(){
	_knn_queries.clear();
	_symmetric_matrix.clear();
	_row_normalization.clear();
	_beta.clear();
	_knn_precision.clear();
	_exaggeration_factors.clear();
	_N = 0;
	_normalization_factor = 0;
}

template <class T>
void KNNSparseMatrix<T>::Resize(int N){
	_knn_queries.resize(N);
	_symmetric_matrix.resize(N);
	_row_normalization.resize(N);
	_beta.resize(N);
	_knn_precision.resize(N);
	_exaggeration_factors.resize(N,1);
	_N = N;
	_normalization_factor = 0;
}

template <class T>
void KNNSparseMatrix<T>::ExageratePointNeighborhood(int n, T factor){
	_exaggeration_factors[n] = factor;

	//std::queue<std::pair<int,int> > queue;
	for(auto neighbour: _knn_queries[n]){
		_exaggeration_factors[neighbour.first] = factor;
	}

}


template <class T>
bool AreEqual(const KNNSparseMatrix<T>& a, const KNNSparseMatrix<T>& b, T eps){
	if(a._N != b._N)
		return false;

	//row normalization
	for(int i = 0; i < a._N; ++i){
		if(std::fabs(a._row_normalization[i] - b._row_normalization[i]) > eps){
			return false;
		}
	}

	//beta
	for(int i = 0; i < a._N; ++i){
		if(std::fabs(a._beta[i] - b._beta[i]) > eps){
			return false;
		}
	}

	//NO exact

	//knn queries
	for(int i = 0; i < a._N; ++i){
		if(a._knn_queries[i].size() != b._knn_queries[i].size()){
			return false;
		}
		for(auto e: a._knn_queries[i]){
			auto find = b._knn_queries[i].find(e.first);
			if(find == b._knn_queries[i].end()){
				return false;
			}
			if(std::fabs(e.second - find->second) > eps){
				return false;
			}
		}
	}

	//symmetric matrix
	for(int i = 0; i < a._N; ++i){
		//if(a._symmetric_matrix[i].size() != b._symmetric_matrix[i].size()){
		//	return false;
		//}
		for(auto e: a._symmetric_matrix[i]){
			auto& ft_a = std::get<0>(e.second);
			auto& st_a = std::get<1>(e.second);
			
			if(std::get<1>(ft_a) == 0. &&  std::get<1>(st_a) == 0.){
				continue;
			}

			auto find = b._symmetric_matrix[i].find(e.first);
			if(find == b._symmetric_matrix[i].end()){
				return false;
			}
			auto& ft_b = std::get<0>(find->second);
			auto& st_b = std::get<1>(find->second);

			if(std::get<0>(ft_a) != std::get<0>(ft_b)){
				return false;
			}
			if(std::get<0>(st_a) != std::get<0>(st_b)){
				return false;
			}

			if(std::fabs(std::get<1>(ft_a) - std::get<1>(ft_b)) > eps){
				return false;
			}
			if(std::fabs(std::get<1>(st_a) - std::get<1>(st_b)) > eps){
				return false;
			}
		}
	}

	//normalization factor
	if(std::fabs(a._normalization_factor - b._normalization_factor) > eps){
		return false;
	}

	return true;
}

//exact init...
template <class T>
void InitializeMatrixWithKNNQueries(const std::vector<std::map<int,double> >& knn_queries, KNNSparseMatrix<T>& sparse_matrix, double perplexity){

	int N = knn_queries.size();

	sparse_matrix._N = N;
	sparse_matrix._knn_queries.resize(N);
	sparse_matrix._row_normalization.resize(N);
	sparse_matrix._beta.resize(N);
	sparse_matrix._knn_precision.resize(N);
	sparse_matrix._exaggeration_factors.resize(N);
	sparse_matrix._symmetric_matrix.resize(N);

	for(int n = 0; n < N; ++n){
		sparse_matrix._knn_precision[n] = 1.;
		sparse_matrix._exaggeration_factors[n] = 10.+N/5000.;

		auto& knn = knn_queries[n];
		std::vector<double> cur_P(knn.size());

		// Initialize some variables for binary search
		bool found = false;
		double beta = 1.0;
		double min_beta = -std::numeric_limits<double>::max();//-DBL_MAX;
		double max_beta = std::numeric_limits<double>::max();//DBL_MAX;
		double tol = 1e-5;

		
		auto knn_iter = knn.begin();
		auto cur_P_iter = cur_P.begin();

		// Iterate until we found a good perplexity
		int iter = 0; double sum_P = 0;
		while(!found && iter < 200) {
			
			// Compute Gaussian kernel row
			//for(int m = 0; m < K; m++)
			for(	knn_iter = knn.begin(), cur_P_iter = cur_P.begin(); 
					knn_iter != knn.end() && cur_P_iter != cur_P.end();
					++knn_iter,++cur_P_iter
				){
					*cur_P_iter = exp(-beta * knn_iter->second);
			}

			// Compute entropy of current row
//			sum_P = DBL_MIN;
			sum_P = std::numeric_limits<double>::min();
			//for(int m = 0; m < K; m++)
			for(auto v: cur_P){
				sum_P += v;
			}

			double H = .0;
			for(	knn_iter = knn.begin(), cur_P_iter = cur_P.begin(); 
					knn_iter != knn.end() && cur_P_iter != cur_P.end();
					++knn_iter,++cur_P_iter
				){
					H += beta * (knn_iter->second * (*cur_P_iter));
			}
			H = (H / sum_P) + log(sum_P);

			// Evaluate whether the entropy is within the tolerance level
			double Hdiff = H - log(perplexity);
			if(Hdiff < tol && -Hdiff < tol) {
				found = true;
			}
			else {
				if(Hdiff > 0) {
					min_beta = beta;
//					if(max_beta == DBL_MAX || max_beta == -DBL_MAX)
                    if(max_beta == std::numeric_limits<double>::max() || max_beta == -std::numeric_limits<double>::max())
						beta *= 2.0;
					else
						beta = (beta + max_beta) / 2.0;
				}
				else {
					max_beta = beta;
//					if(min_beta == -DBL_MAX || min_beta == DBL_MAX)
                    if(min_beta == -std::numeric_limits<double>::max() || min_beta == std::numeric_limits<double>::max())
						beta /= 2.0;
					else
						beta = (beta + min_beta) / 2.0;
				}
			}
			//_beta[n] = beta;
			// Update iteration counter
			iter++;
		}

		sparse_matrix._row_normalization[n] = sum_P;
		sparse_matrix._beta[n] = beta;
		sparse_matrix._knn_precision[n] = 1; //TEMP
		for(	knn_iter = knn.begin(), cur_P_iter = cur_P.begin(); 
				knn_iter != knn.end() && cur_P_iter != cur_P.end();
				++knn_iter,++cur_P_iter
			){
				sparse_matrix._knn_queries[n][knn_iter->first] = *cur_P_iter;
		}
	}

	for(int n = 0; n < N; n++){
		for(auto& e: sparse_matrix._knn_queries[n]){
			auto find	= sparse_matrix._knn_queries[e.first].find(n);
			auto end	= sparse_matrix._knn_queries[e.first].end();
			if(find == end){
				sparse_matrix._symmetric_matrix[n][e.first] = std::make_tuple(std::make_tuple(n,e.second),	std::make_tuple(e.first,0.));
				sparse_matrix._symmetric_matrix[e.first][n] = std::make_tuple(std::make_tuple(e.first,0.),	std::make_tuple(n,e.second));
			}else{
				sparse_matrix._symmetric_matrix[n][e.first] = std::make_tuple(std::make_tuple(n,e.second),				std::make_tuple(e.first,find->second));
				sparse_matrix._symmetric_matrix[e.first][n] = std::make_tuple(std::make_tuple(e.first,find->second),	std::make_tuple(n,e.second));
			}
		}
	}
	sparse_matrix.ComputeNormalizationFactor();
}




#endif
