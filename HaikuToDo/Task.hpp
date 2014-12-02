#ifndef TASK_HPP
#define TASK_HPP

#include <InterfaceKit.h>

class Task : public BStringItem{
	public:
		Task(const char* text,const char* details,bool completed) : 
			BStringItem(text),
			details(details),
			completed(completed)
		{
			
		}
		
		const char*
		GetDetails()
		{
			return details.String();
		}
		
		bool
		GetCompleted()
		{
			return completed;
		}
	private:
		BString details;
		bool completed;
};

#endif
