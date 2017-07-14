#ifndef COUT_LOG_H
#define COUT_LOG_H

#include <string>
#include <sstream>
#include "log/abstract_log.h"

namespace nut{

	//! shows log on the standar output stream
	class CoutLog: public AbstractLog{
	public:
		CoutLog():num_written_chars_(0){}
	    virtual ~CoutLog(){}
		//! set the number of written characters to zero
	    virtual void Clear();
		//! display some text on the logger
	    virtual void Display(const std::string& d);
	    //! return the number of written character
	    unsigned int num_written_chars()const;
	    	
	private:
		unsigned int num_written_chars_;
	};
	
}

//Implementation
#include "cout_log-inl.h"

#endif // COUT_LOG_H
