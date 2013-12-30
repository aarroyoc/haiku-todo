#ifndef GTASKSTASKSYNCHRONIZER_H
#define GTASKSTASKSYNCHRONIZER_H


#include "TaskSynchronizer.h"


namespace Plugin {

class GTasksTaskSynchronizer
	:
	public PluginEngine::TaskSynchronizer
{
	public:
							GTasksTaskSynchronizer::GTasksTaskSynchronizer(
								bool synchronizationEnabled = true,
								int32 synchronizationTimestampSeconds = 60);
		virtual				~GTasksTaskSynchronizer();
	
	
	protected:
		// Methods to define in plugin for synchronization.
		virtual bool		_OnSynchronizeAll();
		virtual bool		_OnSynchronizeList(AppEngine::TaskList& list);
		virtual bool		_OnSynchronizeTask(AppEngine::Task& task);
};

} // namespace Plugin

#endif // GTASKSTASKSYNCHRONIZER_H
