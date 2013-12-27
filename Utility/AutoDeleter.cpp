#include "AutoDeleter.h"

#include <cassert>


template<typename T>
AutoDeleter<T>::AutoDeleter(T* object, Deleter deleter)
	:
	fObject(object),
	fDeleter(deleter)
{
	assert(fObject != nullptr);
	assert(fDeleter != nullptr);
}


template<typename T>
AutoDeleter<T>::~AutoDeleter()
{
	fDeleter(fObject);
}
