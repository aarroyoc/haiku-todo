#include "AutoDeleter.h"

#include <cassert>



//////////////////////////////////////////////////////////////////////////////
//
//	Static methods
//
//////////////////////////////////////////////////////////////////////////////


template<typename T>
void
AutoDeleter<T>::DefaultDeleter(T* object)
{
	delete object;	
}


//////////////////////////////////////////////////////////////////////////////
//
//	Constructor & destructor
//
//////////////////////////////////////////////////////////////////////////////


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


//////////////////////////////////////////////////////////////////////////////
//
//	Public methods - getters
//
//////////////////////////////////////////////////////////////////////////////


template<typename T>
T*
AutoDeleter<T>::GetPointer() const
{
	return fObject;	
}


//////////////////////////////////////////////////////////////////////////////
//
//	Public methods - modifiers
//
//////////////////////////////////////////////////////////////////////////////


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


template<typename T>
void
AutoDeleter<T>::Reset(T* object, Deleter deleter)
{
	this->Swap(AutoDeleter<T>(object, deleter));
		// Old object will be deleted (as temporary object)
}
