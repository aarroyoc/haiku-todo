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

std::map<BString, Task*> Task::sExistingTasks;
BLocker Task::sExistingTasksMutex("Task::sExistingTasks mutex");


void
Task::_Register(Task& task)
{
	BAutolock guard(sExistingTasksMutex);
	assert(sExistingTasks.find(task.GetId()) == sExistingTasks.end());
		// One task can be registered only once
	sExistingTasks[task.GetId()] = &task;
}


void
Task::_Unregister(Task& task)
{
	BAutolock guard(sExistingTasksMutex);
	assert(sExistingTasks.find(task.GetId()) != sExistingTasks.end());
		// Task must be registered to unregister
	sExistingTasks.erase(task.GetId());
}


Task*
Task::GetById(BString Id)
{
	static bool inited = false;
	if(!inited) {
		sExistingTasks[""] = nullptr;
		inited = true;	
	}
	
	BAutolock guard(sExistingTasksMutex);
	assert(sExistingTasks.find(Id) != sExistingTasks.end());
		// consider this assertion
	return sExistingTasks[Id];
}


//////////////////////////////////////////////////////////////////////////////
//
//	Non-static members
//	- constructors
//	- destructor
//
//////////////////////////////////////////////////////////////////////////////


Task::Task(TaskList& owner, BString title, BString notes, time_t dueDate)
	:
	fTitle(title),
	fNotes(notes),
	fDueDate(dueDate),
	fId(GetNextId()),
	fCompleted(false),
	fDeleted(false),
	// skipping parent, children and siblings,
	// default constructor is quite fine
	fOwner(owner),
	fLastUpdate(0),
	fLastLocalChange(0),
	fLastUpdateCopy(),
	fMutex(("Task mutex, id: " + fId).String()),
	fIsCopyObject(false)
{
	_Register(*this);
}


Task::~Task()
{
	if(fIsCopyObject)
		return;
	
	_Unregister(*this);
	_DeleteNode();
}


Task::Task(const Task& pattern)
	:
	fTitle(pattern.fTitle),
	fNotes(pattern.fNotes),
	fDueDate(pattern.fDueDate),
	fId(pattern.fId),
	fCompleted(pattern.fCompleted),
	fDeleted(pattern.fDeleted),
	fParentId(pattern.fParentId),
	fFirstChildId(pattern.fFirstChildId),
	fNextSiblingId(pattern.fNextSiblingId),
	fPreviousSiblingId(pattern.fPreviousSiblingId),
	fOwner(pattern.fOwner),
	
	fLastUpdate(0),
	fLastLocalChange(0),
	fLastUpdateCopy(),
	fMutex(("[COPY] Task mutex, id" + fId).String()),
	fIsCopyObject(true)
{
}


//////////////////////////////////////////////////////////////////////////////
//
//	Non-static members
//	- public getters
//
//////////////////////////////////////////////////////////////////////////////


BString
Task::GetTitle() const
{
	BAutolock guard(fMutex);
	return fTitle;
}


BString
Task::GetNotes() const
{
	BAutolock guard(fMutex);
	return fNotes;
}


time_t
Task::GetDueDate() const
{
	BAutolock guard(fMutex);
	return fDueDate;
}


bool
Task::IsCompleted() const
{
	BAutolock guard(fMutex);
	return fCompleted;
}


bool
Task::IsDeleted() const
{
	BAutolock guard(fMutex);
	return fDeleted;
}


BString
Task::GetId() const
{
	BAutolock guard(fMutex);
	return fId;
}


Task*
Task::GetParent() const
{
	BAutolock guard(fMutex);
	return GetById(fParentId);
}


Task*
Task::GetFirstChild() const
{
	BAutolock guard(fMutex);
	return GetById(fFirstChildId);
}


Task*
Task::GetPreviousSibling() const
{
	BAutolock guard(fMutex);
	return GetById(fPreviousSiblingId);
}


Task*
Task::GetNextSibling() const
{
	BAutolock guard(fMutex);
	return GetById(fNextSiblingId);
}


TaskList&
Task::GetOwner() const
{
	// current object does *NOT* change owner, so we don't need thread-safety
	return fOwner;	
}

//////////////////////////////////////////////////////////////////////////////
//
//	Non-static members
//	- public setters
//
//////////////////////////////////////////////////////////////////////////////


void
Task::SetTitle(BString title)
{
	assert(fIsCopyObject == false);
	BAutolock guard(fMutex);
	fTitle = title;	
}


void
Task::SetNotes(BString notes)
{
	assert(fIsCopyObject == false);
	BAutolock guard(fMutex);
	fNotes = notes;	
}


void
Task::SetDueDate(time_t dueDate)
{
	assert(fIsCopyObject == false);
	BAutolock guard(fMutex);
	fDueDate = dueDate;	
}


void
Task::Complete(bool state, bool affectChildren, bool affectParent)
{
	assert(fIsCopyObject == false);
	BAutolock guard(fMutex);
	fCompleted = state;
	
	if (affectChildren == true) {
		// Mark all children recursively as done.
		Task* child = GetById(fFirstChildId);
		for ( ; child != nullptr; child = child->GetNextSibling())
				child->Complete(state, true, false);
	}
	
	if (state == false && affectParent == true) {
		Task* parent = GetById(fParentId);
		if (parent)
			parent->Complete(false, false, false);
	}
}


void
Task::Delete()
{
	assert(fIsCopyObject == false);
	BAutolock guard(fMutex);
	fDeleted = true;
	
	// Delete all children recursively.
	Task* child = GetById(fFirstChildId);
	for ( ; child != nullptr; child = child->GetNextSibling())
			child->Delete();
}


void
Task::SetParent(Task* parent)
{
	assert(fIsCopyObject == false);
	BAutolock guard(fMutex);
	_DeleteNode();
	_InsertNode(nullptr, parent);	
}


void
Task::SetPreviousSibling(Task* sibling)
{
	assert(fIsCopyObject == false);
	BAutolock guard(fMutex);
	_DeleteNode();
	_InsertNode(sibling, sibling ? sibling->GetParent() : GetById(fParentId));
}


//////////////////////////////////////////////////////////////////////////////
//
//	Non-static members
//	- private methods
//
//////////////////////////////////////////////////////////////////////////////


// Reserved for synchronizer (when local id is replaced by this from server)
void
Task::_ChangeId(BString id)
{
	assert(fIsCopyObject == false);
	BAutolock guard(fMutex);
	
	// Re-register object
	_Unregister(*this);
		// ^ Consider it. Do let store old id?
	fId = id;
	_Register(*this);
	
	// Update ID in relatives
	Task* parent = GetById(fParentId);
	if (parent)
		parent->_SetVarFirstChild(this, false);
	Task* previousSibling = GetById(fPreviousSiblingId);
	if (previousSibling)
		previousSibling->_SetVarNextSibling(this, false);
	Task* nextSibling = GetById(fNextSiblingId);
	if (nextSibling)
		nextSibling->_SetVarPreviousSibling(this, false);
}


// Mutex should be locked while calling this function, so we are thread-safe
// and we cannot lock mutex (use thread-safe methods), cause we'll get locked
// forever.
void
Task::_DeleteNode()
{
	Task* previousSibling = GetPreviousSibling();
	Task* nextSibling = GetNextSibling();
	
	// Thread-problem: someone is iterating through siblings and: we lock
	// mutex, neighbor returns pointer to us, we modify this neighbor,
	// but iterating stops on us or - worse - is going from our new position!
	// How to fix it? Mutex on siblings/parent-child connections?
	if (nextSibling)
		nextSibling->_SetVarPreviousSibling(previousSibling);
	if (previousSibling)
		previousSibling->_SetVarNextSibling(nextSibling);
	else { // We are first child, so we should update parent's information
		Task* parent = GetParent();
		if (parent)
			parent->_SetVarFirstChild(nextSibling);
	}
	fPreviousSiblingId = fNextSiblingId = fParentId = "";
}


// Mutex should be locked while calling this function, so we are thread-safe
// and we cannot lock mutex (use thread-safe methods), cause we'll get locked
// forever.
void
Task::_InsertNode(Task* previousSibling, Task* parent)
{
	fParentId = parent ? parent->GetId() : "";
	Task* nextSibling = nullptr;
	if (previousSibling) {
		assert(previousSibling->GetParent() == parent);
		
		// We must lock previousSibling here, because we get it's next sibling
		// and then set it - we cannot let someone read that data in middle
		// time, cause it'll be inconsistent.
		BAutolock(previousSibling->fMutex);
		
		fPreviousSiblingId = previousSibling->fId;
		nextSibling = GetById(previousSibling->fNextSiblingId);
		fNextSiblingId = nextSibling ? nextSibling->GetId() : "";
			
		previousSibling->fNextSiblingId = fId;
	}
	else {
		if (parent) {
			Task* nextSibling = parent->GetFirstChild();
			parent->_SetVarFirstChild(this, false);
			
			fNextSiblingId = nextSibling ? nextSibling->GetId() : "";
		}
		else { // We are new root - so we have to find last one to replace it
			if(!fOwner.GetTaskList().empty()) {
				nextSibling = fOwner.GetRootTask();
				fNextSiblingId = nextSibling->GetId();
			}
			else { // There's no root (empty TaskList)
				nextSibling = nullptr;
			}
		}
	}
	
	if (nextSibling)
		nextSibling->_SetVarPreviousSibling(this, false);
}


void
Task::_SetVarNextSibling(Task* sibling, bool targetSafeLock)
{
	assert(fIsCopyObject == false);
	BAutolock guard(fMutex);
	if (sibling == nullptr)
		fNextSiblingId = "";
	else
		fNextSiblingId = targetSafeLock ? sibling->GetId() : sibling->fId;
}


void
Task::_SetVarPreviousSibling(Task* sibling, bool targetSafeLock)
{
	assert(fIsCopyObject == false);
	BAutolock guard(fMutex);
	if (sibling == nullptr)
		fPreviousSiblingId = "";
	else
		fPreviousSiblingId = targetSafeLock ? sibling->GetId() : sibling->fId;
}


void
Task::_SetVarFirstChild(Task* child, bool targetSafeLock)
{
	assert(fIsCopyObject == false);
	BAutolock guard(fMutex);
	if (child == nullptr)
		fFirstChildId = "";
	else
		fFirstChildId = targetSafeLock ? child->GetId() : child->fId;
}

	
} // namespace AppEngine
