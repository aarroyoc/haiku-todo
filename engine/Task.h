#ifndef TASK_H
#define TASK_H


#include <ctime>
#include <map>
#include <memory>
#include <string>

#include <Locker.h>


namespace engine {

class TaskList;

class Task {
	public:
		Task(TaskList& owner, std::string title = "", std::string notes = "",
			time_t dueDate = 0);
		~Task();
		
		void						SetTitle(std::string title);
		std::string					GetTitle() const;
		void						SetNotes(std::string notes);
		std::string					GetNotes() const;
		void						SetDueDate(time_t date);
		time_t						GetDueDate() const;
		void						Complete(bool state,
										bool affectChildren = true,
										bool affectParent = true);
		bool						IsCompleted() const;
		void						Delete();
		bool						IsDeleted() const;
		std::string					GetId() const;
		
		void						SetParent(Task* parent);
										// nullptr means no parent (top tasks)
		Task*						GetParent() const;
		Task*						GetFirstChild() const;
		void						SetPreviousSibling(Task* sibling);
										// nullptr means that this task is
										// first in parent's child list
		Task*						GetPreviousSibling() const;
		Task*						GetNextSibling() const;
		
		TaskList&					GetOwner() const;
		
		
	public:
		static Task*				GetById(std::string Id);
			// Returns pointer instead reference, cause empty Id represents
			// null pointer (nullptr).
	
	private:	
		// These functions create copy object (other behavior). Consider making
		// these public - should object be non-copyable? It has unique id
									Task(const Task& pattern);
		Task&						operator=(const Task& pattern) = delete;
		
		// _ChangeId() is thread-safe
		void						_ChangeId(std::string id);
		// Two methods above are *NOT* thread-safe (it's desired)
		void						_DeleteNode();
		void						_InsertNode(Task* previousSibling,
										Task* parent);
		
		// Some function for reporting changes! (TODO)
		
		// Functions provided only for thread-safety
		void						_SetVarNextSibling(Task* sibling,
										bool targetSafeLock = true);
		void						_SetVarPreviousSibling(Task* sibling,
										bool targetSafeLock = true);
		void						_SetVarFirstChild(Task* child,
										bool targetSafeLock = true);
		
	private:
		std::string					fTitle;
		std::string					fNotes;
		time_t						fDueDate;
		std::string					fId;
		bool						fCompleted;
		bool						fDeleted;
		
		std::string					fParentId;
		std::string					fFirstChildId;
		std::string					fNextSiblingId;
		std::string					fPreviousSiblingId;
		TaskList&					fOwner;
		
		// Variables for synchronizer (not need thread safety)
		time_t						fLastUpdate;
			// if equals zero, then task is not created in online base,
			// so after this creating, id will have new value
		time_t						fLastLocalChange;
		std::unique_ptr<Task>		fLastUpdateCopy;
			// uses for diff update (better synchronization)
		
		mutable BLocker				fMutex;
		const bool					fIsCopyObject;
		
	private:
		static void					_Register(Task& task);
		static void					_Unregister(Task& task);
		static std::map<std::string, Task*>
									sExistingTasks;
		static BLocker				sExistingTasksMutex;
		
		
		
		friend class TaskList;
};

} // namespace engine

#endif // TASK_H
