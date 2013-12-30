#ifndef TASKLISTMANAGER_H
#define TASKLISTMANAGER_H


#include "PluginLoader.h"
#include "TaskList.h"


namespace PluginEngine {

class TaskSynchronizer;
class TaskSerializer;
	
} // namespace PluginEngine

namespace AppEngine {
	
class TaskListManager {	
	public:
		typedef std::list<std::unique_ptr<TaskList>> TaskListContainer;
		
		enum class InitType {
			PluginFilename,
			ContentFilename,
			SerializedData
		};
		
		
	public:
		// Constructor - class is able to hold only one plugin during lifetime,
		//   so you must decide what you will be using before creating object.
		//   Changing plugin during lifetime does *NOT* make sense, because it
		//   can cause synchronization problems - how do you want synchronized
		//   two online lists? If you want to support second list, simple
		//   create new object.
									TaskListManager(InitType type,
										BString data);
		
		const TaskListContainer&	GetLists() const;
		TaskList*					AddList(BString title = "Unnamed list");
		
		void						ClearDeletedLists();
		
		BString						Serialize() const;
		bool						SaveToFile(BString filename) const;
			// File/Serialize format: first line contain plugin's filename,
			// other data are serialized tasks.
		
	private:
		// On fail TaskListManager will be at default state
		// (default plugin with no tasks)
		bool						_Parse(BString serializedData);
		bool						_ReadFromFile(BString filename);
		
		void						_OnUpdate(Task& task) const;
		void						_OnUpdate(TaskList& list) const;
		
		PluginEngine::TaskSynchronizer&
									_GetSynchronizer() const;
			// Should it be public? SynchronizeAll() outside system for a wish
		PluginEngine::TaskSerializer&
									_GetSerializer() const;
		
	private:
		TaskListContainer			fTaskLists;
		PluginEngine::PluginLoader	fPluginLoader;
		
		
		
		friend class Task;
		friend class TaskList;
		friend class PluginEngine::TaskSerializer;
};

} // namespace AppEngine


#endif //TASKLISTMANAGER_H
