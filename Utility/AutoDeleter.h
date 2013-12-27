#ifndef AUTODELETER_H
#define AUTODELETER_H


/*!
 * Template dedicated for plugins and other out-of-application allocations.
 * When plugin allocate new object, it also must provide adequate deleter.
 */

template<typename T>
class AutoDeleter {
	public:
		typedef void (*Deleter)(T*);	
		
									AutoDeleter(T* object, Deleter deleter);
									~AutoDeleter();
		
		T* const					fObject;
		
	private:
		const Deleter				fDeleter;
			// Do not let delete object "by accident"
};

#endif // AUTODELETER_H
