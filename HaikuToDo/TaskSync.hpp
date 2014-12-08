#ifndef TASKSYNC_HPP
#define TASKSYNC_HPP

#include <InterfaceKit.h>

class TaskSync {
	public:
						TaskSync(){};
						~TaskSync(){};
		virtual bool	Login(){};
		virtual void	NextStep(BString code){};
};

#endif
