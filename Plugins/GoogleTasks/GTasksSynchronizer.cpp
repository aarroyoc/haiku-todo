#include "GTasksSynchronizer.h"


namespace Plugin {

//////////////////////////////////////////////////////////////////////////////
//
//	Constructor
//
//////////////////////////////////////////////////////////////////////////////


GTasksSynchronizer::GTasksSynchronizer(
	bool synchronizationEnabled,
	int32 synchronizationTimestampSeconds)
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
GTasksSynchronizer::_OnSynchronizeAll()
{
	return false;
}


bool
GTasksSynchronizer::_OnSynchronizeList(AppEngine::TaskList& list)
{
	return false;
}


bool
GTasksSynchronizer::_OnSynchronizeTask(AppEngine::Task& task)
{
	return false;
}


} // namespace Plugin
