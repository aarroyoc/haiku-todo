#ifndef TASK_H
#define TASK_H


#include <ctime>
#include <map>
#include <memory>

#include <Locker.h>
#include <String.h>


namespace AppEngine {

class TaskList;

class Task {
	public:
									Task(TaskList& owner,
										BString title = "",
										BString notes = "",
										time_t dueDate = 0);
									~Task();
		
		void						SetTitle(BString title);
		BString						GetTitle() const;
		void						SetNotes(BString notes);
		BString						GetNotes() const;
		void						SetDueDate(time_t date);
		time_t						GetDueDate() const;
		void						Complete(bool state,
										bool affectChildren = true,
										bool affectParent = true);
		bool						IsCompleted() const;
		void						Delete();
		bool						IsDeleted() const;
		BString						GetId() const;
		
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
		static Task*				GetById(BString Id);
			// Returns pointer instead reference, cause empty Id represents
			// null pointer (nullptr).
	
	private:	
		// These functions create copy object (other behavior). Consider making
		// these public - should object be non-copyable? It has unique id
									Task(const Task& pattern);
		Task&						operator=(const Task& pattern) = delete;
		
		// _ChangeId() is thread-safe
		void						_ChangeId(BString id);
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
		BString						fTitle;
		BString						fNotes;
		time_t						fDueDate;
		BString						fId;
		bool						fCompleted;
		bool						fDeleted;
		
		BString						fParentId;
		BString						fFirstChildId;
		BString						fNextSiblingId;
		BString						fPreviousSiblingId;
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
		static std::map<BString, Task*>
									sExistingTasks;
		static BLocker				sExistingTasksMutex;
		
		
		
		friend class TaskList;
};

} // namespace AppEngine

#endif // TASK_H
