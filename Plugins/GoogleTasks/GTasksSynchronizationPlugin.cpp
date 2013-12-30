#include "GTasksSynchronizationPlugin.h"
#include "GTasksSynchronizer.h"
#include "TaskSerializer.h"


namespace Plugin {

extern "C"
void CreateSynchronizationPlugin(
	AutoDeleter<PluginEngine::SynchronizationPlugin>& plugin)
{
	AutoDeleter<PluginEngine::TaskSynchronizer> synchronizer
		(new GTasksSynchronizer());
	AutoDeleter<PluginEngine::TaskSerializer> serializer
		(new PluginEngine::TaskSerializer());
				
	PluginEngine::SynchronizationPlugin* pluginPtr
		= new PluginEngine::SynchronizationPlugin(synchronizer, serializer);
	plugin.Reset(pluginPtr);
}


} // namespace Plugin
