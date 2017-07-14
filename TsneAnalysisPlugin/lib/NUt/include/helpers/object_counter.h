#ifndef OBJECT_COUNTER_H
#define OBJECT_COUNTER_H

namespace nut{

	template <class T>
	class ObjectCounter{
	public:
		//! default constructor
		ObjectCounter();
		//! copy constructor
		ObjectCounter(const ObjectCounter&);
		//! destructor
		~ObjectCounter();

		static unsigned int objects_created();
		static unsigned int objects_alive();

	private:
		static unsigned int objects_created_;
		static unsigned int objects_alive_;
	};
	
}

//Implementation
#include "object_counter-inl.h"

#endif // OBJECT_COUNTER_H
