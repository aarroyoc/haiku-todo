#ifndef TASK_H
#define TASK_H


#include <ctime>
#include <map>
#include <memory>
#include <string>

#include <BLocker.h>


class TaskList;

class Task {
	public:
		Task(TaskList& owner, std::string title = "", std::string notes = "");
		Task(TaskList& owner, std::string title, std::string notes,
			time_t dueDate);
		
		void						SetTitle(std::string title);
		std::string					GetTitle() const;
		void						SetNotes(std::string notes);
		std::string					GetNotes() const;
		void						SetDueDate(time_t date);
		time						GetDueDate() const;
		void						Complete(bool state, bool affectOthers
										= false);
		bool						IsCompleted() const;
		void						Delete();
		bool						IsDeleted() const;
		std::string					GetId() const;
		
		void						SetParent(const Task* parent);
										// nullptr means no parent (top tasks)
		Task*						GetParent() const;
		Task*						GetFirstChild() const;
		void						SetPreviousSibling(const Task* sibling);
										// nullptr means that this task is
										// first in parent's child list
		Task*						GetPreviousSibling() const;
		Task*						GetNextSibling() const;
		
		TaskList&					GetOwner() const;
		
		
	public:
		static Task*				GetById(std::string Id);	
	
	private:
									Task();					
										// creates copy object (other behavior)
		std::unique_ptr<Task>		_CreateCopy() const;
		
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
		
		time_t						fLastUpdate;
			// if equals zero, then task is not created in online base,
			// so after this creating, id will have new value
		time_t						fLastLocalChange;
		std::unique_ptr<Task>		fLastUpdateState;
			// uses for diff update (better synchronization)
		BLocker						fMutex;		
		
	private:
		static Task&				_Register(Task& task, std::string Id);
		static Task&				_Unregister(std::string Id);
		static std::map<std::string, Task*>
									sExistingTasks;
	
};

#endif
