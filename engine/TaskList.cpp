#include "TaskList.h"

#include <cassert>

#include <Autolock.h>


namespace engine {
	

//////////////////////////////////////////////////////////////////////////////
//
//	Static members
//
//////////////////////////////////////////////////////////////////////////////

std::map<std::string, TaskList*> TaskList::sExistingLists;
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
TaskList::GetById(std::string Id)
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

} // namespace engine

