#ifndef COUT_LOG_INL
#define COUT_LOG_INL

#include <iostream>

namespace nut{

	inline void CoutLog::Clear(){
		num_written_chars_ = 0;
	}

	inline void CoutLog::Display(const std::string& d){
		std::cout << d << std::endl;
		num_written_chars_ += d.size() + 1; 
	}

	inline unsigned int CoutLog::num_written_chars()const{
		return num_written_chars_;
	}
	
}

#endif // COUT_LOG_INL
