#ifndef TASKSERIALIZER_H
#define TASKSERIALIZER_H


#include <vector>

#include <String.h>

#include "json.h"


namespace AppEngine {

class TaskListManager;
class TaskList;
class Task;
	
} // namespace AppEngine


namespace PluginEngine {

class TaskSerializer {
	public:
		virtual BString 	Serialize(const AppEngine::TaskListManager&
								manager) const;
		virtual bool		Parse(AppEngine::TaskListManager& manager,
								const BString& data) const;
	
	
	private:
		// This is section for default serialization and parsing - that's why
		// these methods are non-virtual and there're in private section.
		json::Object		_SerializeList(const AppEngine::TaskList& list)
								const;
		json::Object		_SerializeTask(const AppEngine::Task& task) const;
		
		bool				_ParseList(AppEngine::TaskList& list,
								json::Object& listJson) const;
		bool				_ParseTask(AppEngine::Task& task,
								json::Object& taskJson) const;
		bool				_CheckKeys(json::Object& object,
								const std::vector<std::string>& keys) const;
};

} // namespace PluginEngine

#endif // TASKSERIALIZER_H
