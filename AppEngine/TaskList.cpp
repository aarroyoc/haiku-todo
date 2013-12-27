#include "TaskList.h"
#include "TaskUtility.h"

#include <cassert>

#include <Autolock.h>


namespace AppEngine {
	

//////////////////////////////////////////////////////////////////////////////
//
//	Static members
//
//////////////////////////////////////////////////////////////////////////////

std::map<BString, TaskList*> TaskList::sExistingLists;
BLocker TaskList::sExistingListsMutex("Task::sExistingLists mutex");


void
TaskList::_Register(TaskList& list)
{
	BAutolock guard(sExistingListsMutex);
	assert(sExistingLists.find(list.GetId()) == sExistingLists.end());
		// One task can be registered only once
	sExistingLists[list.GetId()] = &list;
}


void
TaskList::_Unregister(TaskList& list)
{
	BAutolock guard(sExistingListsMutex);
	assert(sExistingLists.find(list.GetId()) != sExistingLists.end());
		// Task must be registered to unregister
	sExistingLists.erase(list.GetId());
}


TaskList*
TaskList::GetById(BString Id)
{
	static bool inited = false;
	if(!inited) {
		sExistingLists[""] = nullptr;
		inited = true;	
	}
	
	BAutolock guard(sExistingListsMutex);
	assert(sExistingLists.find(Id) != sExistingLists.end());
		// consider this assertion
	return sExistingLists[Id];
}


//////////////////////////////////////////////////////////////////////////////
//
//	Non-static members
//	- constructors
//	- destructor
//
//////////////////////////////////////////////////////////////////////////////


TaskList::TaskList(TaskListManager& owner, BString title)
	:
	fTitle(title),
	fId(GetNextId()),
	fDeleted(false),
	fTaskList(),
	fLastUpdate(0),
	fLastLocalChange(0),
	fMutex(("[COPY] TaskList mutex, id" + fId).String()),
	fOwner(owner)
{
	_Register(*this);	
}


TaskList::~TaskList()
{
	_Unregister(*this);	
}


//////////////////////////////////////////////////////////////////////////////
//
//	Non-static members
//	- public getters
//
//////////////////////////////////////////////////////////////////////////////


BString
TaskList::GetTitle() const
{
	BAutolock guard(fMutex);
	return fTitle;
}


BString
TaskList::GetId() const
{
	BAutolock guard(fMutex);
	return fId;
}


const TaskList::TaskContainer&
TaskList::GetTaskList() const
{
	BAutolock guard(fMutex);
	return fTaskList;
}


bool
TaskList::IsDeleted() const
{
	BAutolock guard(fMutex);
	return fDeleted;
}


Task*
TaskList::GetRootTask() const
{
	BAutolock guard(fMutex);
	if (fTaskList.empty())
		return nullptr;
		 
	Task* root = &*fTaskList.front();
	while(root->GetParent() != nullptr)
		root = root->GetParent();
	while(root->GetPreviousSibling() != nullptr)
		root = root->GetPreviousSibling();
	return root;
}


TaskListManager&
TaskList::GetOwner() const
{
	BAutolock guard(fMutex);
	return fOwner;
}

//////////////////////////////////////////////////////////////////////////////
//
//	Non-static members
//	- public setters
//
//////////////////////////////////////////////////////////////////////////////


void
TaskList::SetTitle(BString title)
{
	BAutolock guard(fMutex);
	fTitle = title;
}


void
TaskList::Delete()
{
	BAutolock guard(fMutex);
	fDeleted = true;
}


void
TaskList::DeleteCompletedTasks()
{
	BAutolock guard(fMutex);
	// range-based for uses assignment which is forbidden for std::unique_ptr
	for(TaskContainer::iterator it = fTaskList.begin(); it != fTaskList.end();
																		it++)
		(*it)->Delete();	
}


void
TaskList::ClearDeletedTasks()
{
	BAutolock guard(fMutex);
	for(TaskContainer::iterator it = fTaskList.begin(); it != fTaskList.end();)
	{
		bool deletedRecently = false;
		
		BAutolock((*it)->fMutex);
		if ((*it)->fDeleted == true &&
			(*it)->fLastUpdate > (*it)->fLastLocalChange) {
				// delete only when task is deleted on server!
			it = fTaskList.erase(it);
			deletedRecently = true;
		}
		
		if (deletedRecently == false)
			it++;
	}
}


Task*
TaskList::AddTask(Task* parent, BString title, BString notes,
	time_t dueDate)
{
	BAutolock guard(fMutex);
	Task* task = new Task(*this, title, notes, dueDate);
	std::unique_ptr<Task> taskPtr(task);
	fTaskList.push_back(std::move(taskPtr));
	
	task->SetParent(parent);
	Task* lastSibling = task;
	while (lastSibling->GetNextSibling() != nullptr)
		lastSibling = lastSibling->GetNextSibling();
	task->SetPreviousSibling(lastSibling);
	return task;
}


//////////////////////////////////////////////////////////////////////////////
//
//	Non-static members
//	- private methods
//
//////////////////////////////////////////////////////////////////////////////


void
TaskList::_ChangeId(BString id)
{
	BAutolock guard(fMutex);
	
	_Unregister(*this);
	fId = id;
	_Register(*this);
	
	// Since no one stores our id, we have nothing to update		
}

} // namespace AppEngine

