#ifndef TASK_HPP
#define TASK_HPP

#include <InterfaceKit.h>

class Task : public BStringItem{
	public:
		Task(const char* text,const char* details,const char* category,bool completed) : 
			BStringItem(text),
			details(details),
			category(category),
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
		
		const char*
		GetCategory()
		{
			return category.String();
		}
	private:
		BString details;
		bool completed;
		BString category;
};

#endif
