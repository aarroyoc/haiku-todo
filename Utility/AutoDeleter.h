#ifndef AUTODELETER_H
#define AUTODELETER_H


#include <memory>

/*!
 * Template dedicated for plugins and other out-of-application allocations.
 * When plugin allocate new object, it also must provide adequate deleter.
 */

template<typename T>
class AutoDeleter {
	public:
		typedef void (*Deleter)(T*);	
		
									AutoDeleter(T* object = nullptr,
										Deleter deleter = DefaultDeleter);
									~AutoDeleter();
									
		void						Swap(AutoDeleter<T>& other);
		T*							GetObject() const;
		
		static void					DefaultDeleter(T* object);
			// std::default_delete<T> not provided...
		
		
	private:
		T*							fObject;
		Deleter						fDeleter;
};

#endif // AUTODELETER_H
