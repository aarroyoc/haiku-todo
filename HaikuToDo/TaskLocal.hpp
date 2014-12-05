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
			void	LoadTasks(const char* category,BListView* tasks);
			void	LoadCategories(BListView* categories);
			bool	AddCategory(const char* name, const char* filename);
			bool	AddTask(const char* title, const char* description, const char* category);
			bool	RemoveTask(const char* title, const char* description, const char* category);
			bool	MarkAsComplete(const char* title, const char* description, const char* category);
	private:
		sqlite3* 	db;
		BAlert*		error;
};
#endif
