#ifndef GTASKSTASKSYNCHRONIZER_H
#define GTASKSTASKSYNCHRONIZER_H


#include "OAuth2.h"
#include "TaskSynchronizer.h"

#include <vector>


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
		
		
		virtual bool		ProvideExtraData(void* extraData);
	
	
	protected:
		// Methods to define in plugin for synchronization.
		virtual bool		_OnSynchronizeAll();
		virtual bool		_OnSynchronizeList(AppEngine::TaskList& list);
		virtual bool		_OnSynchronizeTask(AppEngine::Task& task);
		
		
	private:
		bool				_PushCreatedTask(AppEngine::Task& task);
		bool				_PushExistingTask(AppEngine::Task& task);
		bool				_PushCreatedList(AppEngine::TaskList& list);
		bool				_PushExistingList(AppEngine::TaskList& list);
		bool				_UpdateCreatedAndDeletedListsAndTasks();
		bool				_UpdateExistingListsAndTasks();
		bool				_UpdateAllExistingTasksOnList(
								AppEngine::TaskList& list);
		
		std::vector<BString>_GetRequestHeaders(bool hasJsonBody = false);
		
		void				_AuthorizationInit();
		
		
	private:
		OAuth2				fAuthorization;
		BString				kRequestUrlBase;
};

} // namespace Plugin

#endif // GTASKSTASKSYNCHRONIZER_H
