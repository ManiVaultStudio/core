#ifndef LOG_HELPER_FUNCTIONS_H
#define LOG_HELPER_FUNCTIONS_H

#include <string>
#include <sstream>

namespace nut{
	template <class Logger>
	inline void SecureLog(Logger* logPtr, const std::string& text, bool enabled = true){
		if(enabled){
			if(logPtr!=nullptr){
				logPtr->Display(text);
			}
		}
	}

	template <class Logger, class Data>
	inline void SecureLogValue(Logger* logPtr, const std::string& text, const Data& v, bool enabled = true){
		if(enabled){
			if(logPtr!=nullptr){
				std::stringstream ss;
				ss << text << ": " << v;
				logPtr->Display(ss.str());
			}
		}
	}

	template <class Logger>
	inline void SecureClear(Logger* logPtr){
		if(logPtr!=nullptr){
			logPtr->Clear();
		}
	}
}

#endif // LOG_HELPER_FUNCTIONS_H