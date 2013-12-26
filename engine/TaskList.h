#ifndef TASKLIST_H
#define TASKLIST_H


#include "Task.h"

#include <list>


namespace engine {

class TaskListManager;

class TaskList {
	public:
		typedef std::list<std::unique_ptr<Task>> TaskContainer;
		
									TaskList(TaskListManager& owner,
										std::string title = "");
								// Non-copyable
									TaskList(const TaskList&) = delete;
		TaskList& 					operator=(const TaskList&) = delete;
		
		void						SetTitle(std::string title);
		std::string					GetTitle() const;
		std::string					GetId() const;
		const TaskContainer &		GetTaskList() const;
		void						Delete();
		void						ClearCompleted();
		void						ClearDeleted();
		Task*						GetFirstTask();
		TaskListManager&			GetOwner();
		template<typename ...T>
		Task*						AddTask(T... constructorArgs);
		
		
	public:
		static TaskList*			GetById(std::string id);
		
		
	private:
		void						_ChangeId(std::string id);
		
		
	private:
		std::string					fTitle;
		std::string					fId;
		
		TaskContainer				fTaskList;
		
		time_t						fLastUpdate;
		time_t						fLastLocalChange;
		
		mutable BLocker				fMutex;
		TaskListManager&			fOwner;
		
		
	private:
		static void					_Register(TaskList& list);
		static void					_Unregister(TaskList& list);
		static std::map<std::string, TaskList*>
									sExistingLists;
		static BLocker				sExistingListsMutex;

};

} // namespace engine

#endif // TASKLIST_H
