#ifndef ASSERT_BY_EXCEPTION_H
#define ASSERT_BY_EXCEPTION_H

#include <cassert>
#include <string>
#include <sstream>

namespace nut{

	template <class Ex>
	inline void CheckAndThrow(bool v, const std::string& text){
		if(!v){
			throw Ex(text);
		}
	}
	inline void CheckAndThrowLogic(bool v, const std::string& text){
		CheckAndThrow<std::logic_error>(v,text);
	}
	inline void CheckAndThrowRuntime(bool v, const std::string& text){
		CheckAndThrow<std::runtime_error>(v,text);	
	}

	template <class Ex>
	inline void CheckAndThrow(bool v, const std::string& text, const std::string& file_name, int line){
		if(!v){
			std::stringstream ss;
			ss << text << std::endl;
			ss << "File: " << file_name <<	std::endl;
			ss << "Line: " << line <<		std::endl;
			throw Ex(ss.str());
		}
	}
	inline void CheckAndThrowLogic(bool v, const std::string& text, const std::string& file_name, int line){
		CheckAndThrow<std::logic_error>(v,text,file_name,line);
	}
	inline void CheckAndThrowRuntime(bool v, const std::string& text, const std::string& file_name, int line){
		CheckAndThrow<std::runtime_error>(v,text,file_name,line);	
	}
	
}

#endif // ASSERT_BY_EXCEPTION_H
