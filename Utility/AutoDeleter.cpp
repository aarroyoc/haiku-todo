#include "AutoDeleter.h"

#include <cassert>


template<typename T>
void
AutoDeleter<T>::DefaultDeleter(T* object)
{
	delete object;	
}


template<typename T>
AutoDeleter<T>::AutoDeleter(T* object, Deleter deleter)
	:
	fObject(object),
	fDeleter(deleter)
{
	if (object)
		assert(deleter != nullptr);
}


template<typename T>
AutoDeleter<T>::~AutoDeleter()
{
	if (fObject != nullptr)
		fDeleter(fObject);
}


template<typename T>
T*
AutoDeleter<T>::GetObject() const
{
	return fObject;	
}


template<typename T>
void
AutoDeleter<T>::Swap(AutoDeleter<T>& other)
{
	T* myObject = fObject;
	Deleter myDeleter = fDeleter;
	
	fObject = other.fObject;
	fDeleter = other.fDeleter;
	
	other.fObject = myObject;
	other.fDeleter = myDeleter;
}
