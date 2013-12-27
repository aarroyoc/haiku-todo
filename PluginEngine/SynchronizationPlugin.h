#ifndef SYNCHRONIZATIONPLUGIN_H
#define SYNCHRONIZATIONPLUGIN_H


#include "AutoDeleter.h"

#include <memory>


namespace PluginEngine {

class TaskSynchronizer;
class TaskSerializer;

class SynchronizationPlugin {
	public:
		// Since we load plugin and gets allocated objects, we need also to
		// delete these objects by adequate deleter.
								SynchronizationPlugin(
									AutoDeleter<TaskSynchronizer>& synchronizer,
									AutoDeleter<TaskSerializer>& serializer);
								~SynchronizationPlugin();
								
	TaskSynchronizer&			GetSynchronizer() const;
	TaskSerializer&				GetSerializer() const;
	
	
	private:	
		AutoDeleter<TaskSynchronizer>
								fSynchronizer;
		AutoDeleter<TaskSerializer>
								fSerializer;
};

} // namespace PluginEngine


#endif // SYNCHRONIZATIONPLUGIN_H
