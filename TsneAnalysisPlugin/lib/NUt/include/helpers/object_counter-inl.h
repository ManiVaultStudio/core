#ifndef OBJECT_COUNTER_INL
#define OBJECT_COUNTER_INL

namespace nut{

	template <class T>
	ObjectCounter<T>::ObjectCounter(){
		++objects_created_;
		++objects_alive_;
	}
	template <class T>
	ObjectCounter<T>::ObjectCounter(const ObjectCounter&){
		++objects_created_;
		++objects_alive_;
	}
	template <class T>
	ObjectCounter<T>::~ObjectCounter(){
		--objects_alive_;
	}

	template <class T>
	unsigned int ObjectCounter<T>::objects_created(){
		return objects_created_;
	}

	template <class T>
	unsigned int ObjectCounter<T>::objects_alive(){
		return objects_alive_;
	}
	
	template <class T> unsigned int ObjectCounter<T>::objects_created_(0);
	template <class T> unsigned int ObjectCounter<T>::objects_alive_(0);
}

#endif // OBJECT_COUNTER_INL
