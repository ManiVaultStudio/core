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

#ifndef TSNE_ERROR_H
#define TSNE_ERROR_H

#include "approx_tsne.h"

template <class Traits = TSNEDefaultTraits>
class TSNEErrorUtils{
public:
	typedef typename Traits::scalar_type scalar_type;
	typedef typename Traits::vector_type vector_type;
	typedef typename Traits::flag_type flag_type;
	typedef typename Traits::sparse_matrix_type sparse_matrix_type;


public:
	static void ComputeExactTSNEError(const sparse_matrix_type& sparse_matrix, const scalar_type* solution, int D, double& total_error, double& min_error, double& max_error);
	static void ComputeBarnesHutTSNEErrorWithTreeComputation(const sparse_matrix_type& sparse_matrix, scalar_type* solution, int D, double& total_error, double& min_error, double& max_error, double theta);
	static void ComputeBarnesHutTSNEError(const sparse_matrix_type& sparse_matrix, const scalar_type* solution, int D, double sumQ, double& total_error, double& min_error, double& max_error);

	static void ComputeBarnesHutTSNEErrorWithTreeComputationOnSubset(const sparse_matrix_type& sparse_matrix, scalar_type* solution, const std::vector<int>& idx, int D, double& total_error, double& min_error, double& max_error, double theta);
	static void ComputeBarnesHutTSNEErrorOnSubset(const sparse_matrix_type& sparse_matrix, const scalar_type* solution, const std::vector<int>& idx, int D, double sumQ, double& total_error, double& min_error, double& max_error);

};

#include "tsne_errors-inl.h"

#endif