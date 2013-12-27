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
		
		// Non-copyable
									AutoDeleter(const AutoDeleter&) = delete;
		AutoDeleter&				operator=(const AutoDeleter&) = delete;
									
									AutoDeleter(T* object = nullptr,
										Deleter deleter = DefaultDeleter);
									~AutoDeleter();
									
		void						Swap(AutoDeleter<T>& other);
		void						Reset(T* object = nullptr,
										Deleter deleter = DefaultDeleter);
		T*							GetPointer() const;
		
		static void					DefaultDeleter(T* object);
			// std::default_delete<T> not provided...
		
		
	private:
		
		
	private:
		T*							fObject;
		Deleter						fDeleter;
};

#endif // AUTODELETER_H
