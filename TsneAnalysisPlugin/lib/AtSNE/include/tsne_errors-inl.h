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

#ifndef TSNE_ERROR_INL
#define TSNE_ERROR_INL

#include "sptree.h"

template <class Traits>
void TSNEErrorUtils<Traits>::ComputeExactTSNEError(const sparse_matrix_type& sparse_matrix, const scalar_type* solution, int D, double& total_error, double& min_error, double& max_error){
}

template <class Traits>
void TSNEErrorUtils<Traits>::ComputeBarnesHutTSNEErrorWithTreeComputation(const sparse_matrix_type& sparse_matrix, scalar_type* solution, int D, double& total_error, double& min_error, double& max_error, double theta){
	SPTree tree(D, solution, sparse_matrix._N);
	double sum_Q = 0;
	std::vector<double> neg_f(sparse_matrix._N*D);
	for(int n = 0; n < sparse_matrix._N; n++){
		tree.computeNonEdgeForces(n, theta, neg_f.data() + n * D, &sum_Q);
	}
	ComputeBarnesHutTSNEError(sparse_matrix,solution,D,sum_Q,total_error,min_error,max_error);
}

template <class Traits>
void TSNEErrorUtils<Traits>::ComputeBarnesHutTSNEError(const sparse_matrix_type& sparse_matrix, const scalar_type* solution, int D, double sumQ, double& total_error, double& min_error, double& max_error){
	std::vector<double> buff(D);
	total_error = 0;
	min_error = std::numeric_limits<double>::max();
	max_error = -std::numeric_limits<double>::max();
	const int N = sparse_matrix._symmetric_matrix.size();


	// Loop over all edges in the graph
	unsigned int ind1, ind2;
	for(unsigned int n = 0; n < N; n++) {
		double point_error = 0.;

		ind1 = n * D;
		for(auto idx: sparse_matrix._symmetric_matrix[n]) {
			double Q = .0;
			ind2 = idx.first * D;
			for(int d = 0; d < D; d++){
				const double val = solution[ind1 + d] - solution[ind2 + d];
				Q+= val*val;
			}

			Q = (1.0 / (1.0 + Q)) / sumQ;
			auto a = std::get<0>(idx.second);
			auto b = std::get<1>(idx.second);
			double p = (std::get<1>(a)/sparse_matrix._row_normalization[std::get<0>(a)] + std::get<1>(b)/sparse_matrix._row_normalization[std::get<0>(b)])/sparse_matrix._normalization_factor;

//			double error = p * log((p + FLT_MIN) / (Q + FLT_MIN));
			double error = p * log((p + std::numeric_limits<float>::min()) / (Q + std::numeric_limits<float>::min()));
			point_error += error;
		}
		max_error = std::max(max_error,point_error);
		min_error = std::max(min_error,point_error);
		total_error += point_error;
	}
	
}



template <class Traits>
void TSNEErrorUtils<Traits>::ComputeBarnesHutTSNEErrorWithTreeComputationOnSubset(const sparse_matrix_type& sparse_matrix, scalar_type* solution, const std::vector<int>& idx, int D, double& total_error, double& min_error, double& max_error, double theta){
	SPTree tree(D, solution, sparse_matrix._N);
	double sum_Q = 0;
	std::vector<double> neg_f(sparse_matrix._N*D);
	for(int n = 0; n < sparse_matrix._N; n++){
		tree.computeNonEdgeForces(n, theta, neg_f.data() + n * D, &sum_Q);
	}
	ComputeBarnesHutTSNEErrorOnSubset(sparse_matrix,solution,idx,D,sum_Q,total_error,min_error,max_error);
}

template <class Traits>
void TSNEErrorUtils<Traits>::ComputeBarnesHutTSNEErrorOnSubset(const sparse_matrix_type& sparse_matrix, const scalar_type* solution, const std::vector<int>& idx, int D, double sumQ, double& total_error, double& min_error, double& max_error){
	std::vector<double> buff(D);
	total_error = 0;
	min_error = std::numeric_limits<double>::max();
	max_error = -std::numeric_limits<double>::max();
	//const int N = sparse_matrix._symmetric_matrix.size();


	// Loop over all edges in the graph
	unsigned int ind1, ind2;
	for(unsigned int i = 0; i < idx.size(); i++) {
		int n = idx[i];

		double point_error = 0.;

		ind1 = n * D;
		double normQ = 0.;
		for(auto idx: sparse_matrix._symmetric_matrix[n]) {
			ind2 = idx.first * D;
			for(int d = 0; d < D; d++){
				double val = solution[ind1 + d] - solution[ind2 + d];
				val = val*val;
				normQ += 1./(1+val);
			}
		}
		for(auto idx: sparse_matrix._symmetric_matrix[n]) {
			double Q = .0;
			ind2 = idx.first * D;
			for(int d = 0; d < D; d++){
				double val = solution[ind1 + d] - solution[ind2 + d];
				val = val*val;
				Q += 1./(1+val);
			}

			Q /= normQ;
			auto a = std::get<0>(idx.second);
			auto b = std::get<1>(idx.second);
			//double p = (std::get<1>(a)/sparse_matrix._row_normalization[std::get<0>(a)] + std::get<1>(b)/sparse_matrix._row_normalization[std::get<0>(b)])/2.;
			double p = std::get<1>(a)/sparse_matrix._row_normalization[std::get<0>(a)];

//			double error = p * log((p + FLT_MIN) / (Q + FLT_MIN));
			double error = p * log((p + std::numeric_limits<float>::min()) / (Q + std::numeric_limits<float>::min()));


			point_error += error;
		}
		max_error = std::max(max_error,point_error);
		min_error = std::max(min_error,point_error);
		total_error += point_error;
	}

	total_error /= idx.size();
	
}

#endif
