#ifndef GTASKSTASKSYNCHRONIZER_H
#define GTASKSTASKSYNCHRONIZER_H


#include "TaskSynchronizer.h"


namespace Plugin {

class GTasksSynchronizer
	:
	public PluginEngine::TaskSynchronizer
{
	public:
							GTasksSynchronizer(
								bool synchronizationEnabled = true,
								int32 synchronizationTimestampSeconds = 60);
		virtual				~GTasksSynchronizer();
	
	
	protected:
		// Methods to define in plugin for synchronization.
		virtual bool		_OnSynchronizeAll();
		virtual bool		_OnSynchronizeList(AppEngine::TaskList& list);
		virtual bool		_OnSynchronizeTask(AppEngine::Task& task);
};

} // namespace Plugin

#endif // GTASKSTASKSYNCHRONIZER_H
