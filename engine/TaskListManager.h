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
		typedef std::list<std::unique_ptr<TaskLists>> TaskListContainer;
		
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
										std::string data);
		
		const TaskListContainer&	GetLists() const;
		void						AddList(std::string title ="Unnamed list");
		
		void						ClearDeletedLists();
		
		std::string					Serialize() const;
		void						SaveToFile(std::string filename) const;
			// File/Serialize format: first line contain plugin's filename,
			// other data are serialized tasks.
		
	private:
		void						_Parse(std::string content);
		void						_ReadFromFile(std::string filename);
		
		TaskSynchronizer&			_GetSynchronizer() const;
			// Should it be public? SynchronizeAll() outside system for a wish
		TaskSerializer&				_GetSerializer() const;	
		
	private:
		TaskListContainer			fTaskLists;
		PluginLoader				fPluginLoader;
};

} // namespace AppEngine


#endif //TASKLISTMANAGER_H
