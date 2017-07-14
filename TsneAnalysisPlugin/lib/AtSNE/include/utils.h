#ifndef UTILS_H
#define UTILS_H

#include "traits.h"

namespace atsne{
	
	template <class Traits = TSNEDefaultTraits>
	class Utils{
	public:
		typedef typename Traits::scalar_type scalar_type;
		typedef typename Traits::vector_type vector_type;
		typedef typename Traits::flag_type flag_type;
		typedef typename Traits::sparse_matrix_type sparse_matrix_type;
	public:
		static void Normalize(scalar_type* X, int N, int D);
		static void ZeroMean(scalar_type* X, int N, int D);
	};

}

#include "utils-inl.h"

#endif
