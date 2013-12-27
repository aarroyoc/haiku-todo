#include "SynchronizationPlugin.h"
#include "TaskSerializer.h"
#include "TaskSynchronizer.h"

#include <cassert>


namespace PluginEngine {

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
	fSynchronizer.Swap(AutoDeleter<TaskSynchronizer>(new TaskSynchronizer));
	fSerializer.Swap(AutoDeleter<TaskSerializer>(new TaskSerializer));
}


SynchronizationPlugin::~SynchronizationPlugin()
{
}


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
