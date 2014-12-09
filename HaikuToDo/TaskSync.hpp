#ifndef TASKSYNC_HPP
#define TASKSYNC_HPP

#include <InterfaceKit.h>
#include <vector>
#include "Category.hpp"
#include "Task.hpp"

class TaskSync {
	public:
							TaskSync(){};
							~TaskSync(){};
		virtual bool		Login(){};
		virtual void		NextStep(BString code){};
		virtual	BList*		GetCategories(){};
		virtual	BList*		GetTasks(Category* category){};
};

#endif
