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
									~TaskList();
								// Non-copyable
									TaskList(const TaskList&) = delete;
		TaskList& 					operator=(const TaskList&) = delete;
		
		void						SetTitle(std::string title);
		std::string					GetTitle() const;
		std::string					GetId() const;
		const TaskContainer &		GetTaskList() const;
		void						Delete();
		bool						IsDeleted() const;
		void						DeleteCompletedTasks();
		void						ClearDeletedTasks();
		Task*						GetRootTask() const;
		TaskListManager&			GetOwner() const;
		Task*						AddTask(Task* parent,
										std::string title = "Unnamed task",
										std::string notes = "",
										time_t dueDate = 0);
		
		
	public:
		static TaskList*			GetById(std::string id);
		
		
	private:
		void						_ChangeId(std::string id);
		
		
	private:
		std::string					fTitle;
		std::string					fId;
		bool						fDeleted;
		
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
