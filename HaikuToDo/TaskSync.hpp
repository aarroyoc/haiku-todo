#ifndef TASKSYNC_HPP
#define TASKSYNC_HPP

#include <InterfaceKit.h>
#include <vector>
#include "Category.hpp"
#include "Task.hpp"

const int32 SYNC_CATEGORIES=800;

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
