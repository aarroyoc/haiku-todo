#ifndef TASKSYNCHRONIZER_H
#define TASKSYNCHRONIZER_H


#include <list>

#include <Locker.h>
#include <StopWatch.h>
#include <SupportDefs.h>


namespace AppEngine {

class TaskListManager;
class TaskList;
class Task;
	
} // namespace AppEngine


namespace PluginEngine {

class TaskSynchronizer {
	public:
							TaskSynchronizer(
								bool synchronizationEnabled = true,
								int32 synchronizationTimestampSeconds = 60);
		virtual				~TaskSynchronizer();
							
			// Object is non-copyable (due to inter alia thread worker)
							TaskSynchronizer(const TaskSynchronizer&) = delete;
		TaskSynchronizer&	operator=(const TaskSynchronizer&) = delete;
	
		void				EnableSynchronization(bool enabled);
		bool				IsSynchronizationEnabled() const;
		void				SetSynchronizationTimestamp(int32 seconds);
		int32				GetSynchronizationTimestamp() const;
		void				SetSynchronizationTarget(
								AppEngine::TaskListManager* manager);
		AppEngine::TaskListManager*
							GetSynchronizationTarget();
		
		void				SynchronizeAll();
		void				SynchronizeList(AppEngine::TaskList& list);
		void				SynchronizeTask(AppEngine::Task& task);
		
		// Add: callbacks with result (failed, updated: changed or not)
		
		
		/// Should be called after running constructor. When this method return
		/// true, it  means that object is properly initialized. When it return
		/// true once, you probably shouldn't call this method anymore (depends
		/// on plugin specification).
		/// ADDIDIONAL INFO: This method was specially provided due to OAuth2
		///    in Google Tasks plugin. There were two ways - listen on given
		///    port and get auth code (no all have forwarded ports) or user'll
		///    get it and should paste it in application.
		//virtual bool		ProvideExtraData(void* extraData);
		
	
	private:
		static int32		_SynchronizationWorker(void* synchronizer);
	
	
	protected:
		// Methods to define in plugin for synchronization.
		// Default behaviour - do nothing.
		virtual bool		_OnSynchronizeAll();
		virtual bool		_OnSynchronizeList(AppEngine::TaskList& list);
		virtual bool		_OnSynchronizeTask(AppEngine::Task& task);
	
		
	private:
		struct Request {
			enum class RequestType {
				SynchronizeTask,
				SynchronizeList,
				SynchronizeAll
			} type;
			union {
				AppEngine::Task*		task;
				AppEngine::TaskList*	list;
			};
		};
		
		
	protected:
		AppEngine::TaskListManager*
							fManager;
	
	
	private:
		bool				fSynchronizationEnabled;
		int32				fSynchronizationTimestamp;
		BStopWatch			fSynchronizationStopwatch;
		thread_id			fWorkerThreadId;
		
		std::list<Request>	fAwaitingRequests;
		
		mutable BLocker		fDataMutex;
		mutable BLocker		fQueueMutex;
			// More mutexes for non-blocking main thread (which can read and
			// set informations about synchronization) and worker thread 
			// (which manage queues).
		
		bool				fDestructorCalled;
			// information for thread worker
};

} // namespace PluginEngine

#endif // TASKSYNCHRONIZER_H
