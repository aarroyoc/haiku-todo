#include "GTasksTaskSynchronizer.h"


namespace Plugin {

//////////////////////////////////////////////////////////////////////////////
//
//	Constructor
//
//////////////////////////////////////////////////////////////////////////////


GTasksTaskSynchronizer::GTasksTaskSynchronizer(
	bool synchronizationEnabled = true,
	int32 synchronizationTimestampSeconds = 60)
	:
	TaskSynchronizer(synchronizationEnabled, synchronizationTimestampSeconds)
{
}


//////////////////////////////////////////////////////////////////////////////
//
//	Synchronization methods
//
//////////////////////////////////////////////////////////////////////////////


bool
GTasksTaskSynchronizer::_OnSynchronizeAll()
{
	return false;
}


bool
GTasksTaskSynchronizer::_OnSynchronizeList(AppEngine::TaskList& list)
{
	return false;
}


bool
GTasksTaskSynchronizer::_OnSynchronizeTask(AppEngine::Task& task)
{
	return false;
}


} // namespace Plugin
