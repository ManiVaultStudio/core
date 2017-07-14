#ifndef INITIALIZATION_HELPER_INL
#define INITIALIZATION_HELPER_INL

#include "errors/invariants.h"
#include "errors/assert_by_exception.h"
namespace nut{

	inline InitializationHelper::InitializationHelper():initialized_(false){

	}
	inline void InitializationHelper::Initialize(){
		PRECONDITION(!IsInitialized());
		initialized_ = true;
		POSTCONDITION(IsInitialized());
	}
	inline void InitializationHelper::Uninitialize(){
		PRECONDITION(IsInitialized());
		initialized_ = false;
		POSTCONDITION(!IsInitialized());	
	}
	inline void InitializationHelper::CheckInitialized()const{
		CheckAndThrowLogic(initialized_,"Initialization check failed!!");
	}
	inline void InitializationHelper::CheckUninitialized()const{
		CheckAndThrowLogic(!initialized_,"Uninitialization check failed!!");
	}
	inline bool InitializationHelper::IsInitialized()const{
		return initialized_;		
	}
}

#endif // INITIALIZATION_HELPER_INL