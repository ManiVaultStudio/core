#ifndef INITIALIZATION_HELPER_H
#define INITIALIZATION_HELPER_H

namespace nut{

	class InitializationHelper{
	public:
		//! Create an unitialized object
		InitializationHelper();
		//! Initialize the helper
		void Initialize();
		//! Deinitialize the helper
		void Uninitialize();
		//! Throw a logic error if the helper is not initialized
		void CheckInitialized()const;
		//! Throw a logic error if the helper is initialized
		void CheckUninitialized()const;
		//! Return the helper state
		bool IsInitialized()const;

	private:
		bool initialized_;
	};

	
}

//Implementation
#include "initialization_helper-inl.h"

#endif // INITIALIZATION_HELPER_H
