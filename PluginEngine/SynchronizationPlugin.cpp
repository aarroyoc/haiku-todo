#include "SynchronizationPlugin.h"
#include "TaskSerializer.h"
#include "TaskSynchronizer.h"

#include <cassert>


namespace PluginEngine {

//////////////////////////////////////////////////////////////////////////////
//
//	Constructor & destructor
//
//////////////////////////////////////////////////////////////////////////////


SynchronizationPlugin::SynchronizationPlugin(
	AutoDeleter<TaskSynchronizer>& synchronizer,
	AutoDeleter<TaskSerializer>& serializer)
{
	assert(synchronizer.GetPointer() != nullptr);
	assert(serializer.GetPointer() != nullptr);
	
	fSynchronizer.Swap(synchronizer);
	fSerializer.Swap(serializer);
}


SynchronizationPlugin::SynchronizationPlugin()
{
	fSynchronizer.Reset(new TaskSynchronizer);
	fSerializer.Reset(new TaskSerializer);
}


SynchronizationPlugin::~SynchronizationPlugin()
{
}


//////////////////////////////////////////////////////////////////////////////
//
//	Public methods - getters
//
//////////////////////////////////////////////////////////////////////////////


TaskSynchronizer&
SynchronizationPlugin::GetSynchronizer() const
{
	return *fSynchronizer.GetPointer();	
}


TaskSerializer&
SynchronizationPlugin::GetSerializer() const
{
	return *fSerializer.GetPointer();	
}


} // namespace PluginEngine
