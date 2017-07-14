#ifndef ABSTRACT_LOG_H
#define ABSTRACT_LOG_H

#include <string>
#include <sstream>

namespace nut{

	//! Abstract interface for a general logger
	class AbstractLog{
	public:
	    virtual ~AbstractLog(){}
		//! clear the log
	    virtual void Clear()=0;
		//! display some text on the logger
	    virtual void Display(const std::string& d)=0;
	};

}
#endif // ABSTRACTLOG_H
