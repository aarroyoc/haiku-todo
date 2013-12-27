#ifndef TASKLIST_H
#define TASKLIST_H


#include "Task.h"

#include <list>


namespace PluginEngine {

class TaskSynchronizer;
class TaskSerializer;
	
} // namespace PluginEngine

namespace AppEngine {

class TaskListManager;

class TaskList {
	public:
		typedef std::list<std::unique_ptr<Task>> TaskContainer;
		
									TaskList(TaskListManager& owner,
										BString title = "");
									~TaskList();
								// Non-copyable
									TaskList(const TaskList&) = delete;
		TaskList& 					operator=(const TaskList&) = delete;
		
		void						SetTitle(BString title);
		BString						GetTitle() const;
		BString						GetId() const;
		const TaskContainer &		GetTaskList() const;
		void						Delete();
		bool						IsDeleted() const;
		void						DeleteCompletedTasks();
		void						ClearDeletedTasks();
		Task*						GetRootTask() const;
		TaskListManager&			GetOwner() const;
		Task*						AddTask(Task* parent,
										BString title = "Unnamed task",
										BString notes = "",
										time_t dueDate = 0);
		
		
	public:
		static TaskList*			GetById(BString id);
		
		
	private:
		void						_ChangeId(BString id);
		
		
	private:
		BString						fTitle;
		BString						fId;
		bool						fDeleted;
		
		TaskContainer				fTaskList;
		
		time_t						fLastUpdate;
		time_t						fLastLocalChange;
		
		mutable BLocker				fMutex;
		TaskListManager&			fOwner;
		
	private:
		static void					_Register(TaskList& list);
		static void					_Unregister(TaskList& list);
		static std::map<BString, TaskList*>
									sExistingLists;
		static BLocker				sExistingListsMutex;
		
		
		
		friend class TaskListManager;
		friend class PluginEngine::TaskSerializer;
};

} // namespace AppEngine

#endif // TASKLIST_H
