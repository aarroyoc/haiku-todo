#include "TaskListManager.h"
#include "TaskSynchronizer.h"

#include "Internet.h"

#include <iostream>
#include <stdexcept>

#include <Autolock.h>
#include <kernel/OS.h>


namespace PluginEngine {

//////////////////////////////////////////////////////////////////////////////
//
//	Constructor & Destructor
//
//////////////////////////////////////////////////////////////////////////////


TaskSynchronizer::TaskSynchronizer(AppEngine::TaskListManager& manager,
	bool synchronizationEnabled, int32 synchronizationTimestampSeconds)
	:
	fManager(nullptr),
	fSynchronizationEnabled(synchronizationEnabled),
	fSynchronizationTimestamp(synchronizationTimestampSeconds),
	fSynchronizationStopwatch("TaskSynchronization Stopwatch", true)
{
	fWorkerThreadId = spawn_thread(_SynchronizationWorker,
		"Task list synchronization worker",	B_NORMAL_PRIORITY, this);
	if (fWorkerThreadId < B_OK)
		throw std::runtime_error("TaskSynchronization::(constructor) - Cannot "
			"create worker thread!");
	resume_thread(fWorkerThreadId);
}


TaskSynchronizer::~TaskSynchronizer()
{
	fDestructorCalled = true;
	status_t exitValue;
	wait_for_thread(fWorkerThreadId, &exitValue);
}


//////////////////////////////////////////////////////////////////////////////
//
//	Public methods - getters
//
//////////////////////////////////////////////////////////////////////////////


bool
TaskSynchronizer::IsSynchronizationEnabled() const
{
	BAutolock guard(fDataMutex);
	return fSynchronizationEnabled && fManager != nullptr;
}


int32
TaskSynchronizer::GetSynchronizationTimestamp() const
{
	BAutolock guard(fDataMutex);
	return fSynchronizationTimestamp;
}


AppEngine::TaskListManager*
TaskSynchronizer::GetSynchronizationTarget() const
{
	BAutolock guard(fDataMutex);
	return fManager;
}


//////////////////////////////////////////////////////////////////////////////
//
//	Public methods - setters
//
//////////////////////////////////////////////////////////////////////////////


void
TaskSynchronizer::EnableSynchronization(bool enabled)
{
	BAutolock guard(fDataMutex);
	fSynchronizationEnabled = enabled;	
}


void
TaskSynchronizer::SetSynchronizationTimestamp(int32 seconds)
{
	BAutolock guard(fDataMutex);
	fSynchronizationTimestamp = seconds;
}


void		
TaskSynchronizer::SetSynchronizationTarget(AppEngine::TaskListManager* manager)
{
	BAutolock guard(fDataMutex);
	fManager = manager;
}


//////////////////////////////////////////////////////////////////////////////
//
//	Public methods - synchronize requests
//
//////////////////////////////////////////////////////////////////////////////


void
TaskSynchronizer::SynchronizeAll()
{
	BAutolock guard(fQueueMutex);

	Request request = { Request::RequestType::SynchronizeAll, { nullptr } };
	fAwaitingRequests.push_front(request);
		// priority request!
}


void
TaskSynchronizer::SynchronizeList(AppEngine::TaskList& list)
{
	BAutolock guard(fQueueMutex);

	Request request = { Request::RequestType::SynchronizeList,
		{ (AppEngine::Task*)&list } };
		// casting to mute warning about missing braces
	fAwaitingRequests.push_back(request);
}


void
TaskSynchronizer::SynchronizeTask(AppEngine::Task& task)
{
	BAutolock guard(fQueueMutex);

	Request request = { Request::RequestType::SynchronizeTask, { &task } };
	fAwaitingRequests.push_back(request);
}


//////////////////////////////////////////////////////////////////////////////
//
//	Private method - synchronization worker (other thread)
//
//////////////////////////////////////////////////////////////////////////////


int32
TaskSynchronizer::_SynchronizationWorker(void* synchronizerPtr)
{
	TaskSynchronizer* synchronizer =
		reinterpret_cast<TaskSynchronizer*>(synchronizerPtr);
	
	bool connectedToInternet = Internet::CheckConnection();
	BAutolock queueGuard(synchronizer->fQueueMutex);
	queueGuard.Unlock();
	
	while (synchronizer->fDestructorCalled == false) {
		
		bigtime_t sleepTimestamp = 1e6; // 1e6 us = 1 sec
		snooze(sleepTimestamp);
		
		if (synchronizer->IsSynchronizationEnabled() == false)
			continue;
		
		
		while (connectedToInternet == false) {
			connectedToInternet = Internet::CheckConnection();
			
			bigtime_t sleepTimestamp = 1e7; // 1e7 us = 10 sec
			snooze(sleepTimestamp);
		}
		
		queueGuard.Lock();
		if (synchronizer->fSynchronizationStopwatch.ElapsedTime() / 1e6
			> synchronizer->GetSynchronizationTimestamp() ||
			(synchronizer->fAwaitingRequests.empty() == false &&
			synchronizer->fAwaitingRequests.front().type ==
			Request::RequestType::SynchronizeAll))
		{
			queueGuard.Unlock();
			if (synchronizer->_OnSynchronizeAll() == true) {
				queueGuard.Lock();
				synchronizer->fAwaitingRequests.clear();
				queueGuard.Unlock();
				synchronizer->fSynchronizationStopwatch.Reset();
			}
			else {
				connectedToInternet = Internet::CheckConnection();
				if (connectedToInternet == false) 
					continue;
				
				std::cerr << "TaskSynchronizer::_SynchronizationWorker: "
					"_OnSynchronizeAll() failed when Internet connection "
					"was available!\n";
				// Callback?
			}
		}
		
		queueGuard.Lock();
		while (synchronizer->fAwaitingRequests.empty() == false) {
			Request request = synchronizer->fAwaitingRequests.front();
			synchronizer->fAwaitingRequests.pop_front();
			queueGuard.Unlock();
			
			bool result = true;
			switch(request.type) {
				case Request::RequestType::SynchronizeList:
					result = synchronizer->_OnSynchronizeList(*request.list);
					break;
					
				case Request::RequestType::SynchronizeTask:
					result = synchronizer->_OnSynchronizeTask(*request.task);
					break;
					
				default:
					break;
			}
			
			if (result == true)
				continue;
			
			
			connectedToInternet = Internet::CheckConnection();
			if (connectedToInternet == false)
				break;
				
			std::cerr << "TaskSynchronizer::_SynchronizationWorker: "
				"_OnSynchronizeList/Task() failed when Internet connection "
				"was available!\n";
			// Callback?
			// Don't panic about data lost - realtime synchronization will
			// update all.
		}
	}
	
	return 0;
}


//////////////////////////////////////////////////////////////////////////////
//
//	Protected methods - default synchronization (stub)
//
//////////////////////////////////////////////////////////////////////////////


bool
TaskSynchronizer::_OnSynchronizeAll()
{
	return true;
}


bool
TaskSynchronizer::_OnSynchronizeList(AppEngine::TaskList& list)
{
	BString title = list.GetTitle();
		// just eliminate "unused variable" warning
	return true;
}


bool
TaskSynchronizer::_OnSynchronizeTask(AppEngine::Task& task)
{
	BString title = task.GetTitle();
		// just eliminate "unused variable" warning
	return true;
}


} // namespace PluginEngine
