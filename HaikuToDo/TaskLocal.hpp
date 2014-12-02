#ifndef TASKLOCAL_HPP
#define TASKLOCAL_HPP

#include <FindDirectory.h>
#include <StorageKit.h>
#include <InterfaceKit.h>
#include <sqlite3.h>
#include <iostream>
#include <cstdlib>


	

class TaskLocal {
	public:
					TaskLocal();
					~TaskLocal();
			void	LoadTasks(BListView* tasks);
			bool	AddTask(const char* title, const char* description);
	private:
		sqlite3* 	db;
		BAlert*		error;
};
#endif
