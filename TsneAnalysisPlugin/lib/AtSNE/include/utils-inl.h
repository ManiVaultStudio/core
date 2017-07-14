#ifndef UTILS_INL_H
#define UTILS_INL_H

#include <limits>

namespace atsne{
	
	template <class Traits>
	void Utils<Traits>::Normalize(scalar_type* X, int N, int D){
		double max_X = std::numeric_limits<double>::min();
		double min_X = std::numeric_limits<double>::max();
		for(int i = 0; i < N * D; i++) {
			if(X[i] > max_X) 
				max_X = X[i];
			if(X[i] < min_X) 
				min_X = X[i];
		}
		for(int i = 0; i < N * D; i++){
			X[i] /= max_X;
		}
	}

	template <class Traits>
	void Utils<Traits>::ZeroMean(scalar_type* X, int N, int D){
		// Compute data mean
		std::vector<double> mean(D,0.);
		for(int n = 0; n < N; n++) {
			for(int d = 0; d < D; d++) {
				mean[d] += X[n * D + d];
			}
		}
		for(int d = 0; d < D; d++) {
			mean[d] /= (double) N;
		}
	
		// Subtract data mean
		for(int n = 0; n < N; n++) {
			for(int d = 0; d < D; d++) {
				X[n * D + d] -= mean[d];
			}
		}
	}
}

#include "utils-inl.h"

#endif
