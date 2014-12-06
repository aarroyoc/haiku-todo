#ifndef TASK_MANAGER_HPP
#define TASK_MANAGER_HPP

class TaskManager{
	public:
		TaskManager()
		{
			
		}
		~TaskManager()
		{
			
		}
		virtual void	LoadTasks(const char* category, BListView* tasks){}
		virtual	void	LoadCategories(BListView* categories){}
		virtual bool 	AddCategory(const char* name, const char* filename){}
		virtual	bool	AddTask(const char* title, const char* description,
							const char* category){}
		virtual	bool	RemoveTask(const char* title, const char* description,
							const char* category){}
		virtual bool	MarkAsComplete(const char* title, const char* description,
							const char* category){}
};

#endif


