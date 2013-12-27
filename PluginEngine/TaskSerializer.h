#ifndef TASKSERIALIZER_H
#define TASKSERIALIZER_H


#include <String.h>


namespace AppEngine {

class TaskListManager;
class TaskList;
class Task;
	
} // namespace AppEngine


namespace json {
	// dedicated for default serialization and parsing
class Object;	
	
} // namespace json


namespace PluginEngine {

class TaskSerializer {
	public:
		virtual BString 	Serialize(const AppEngine::TaskListManager&
								manager) const;
		virtual bool		Parse(AppEngine::TaskListManager& manager) const;
	
	
	private:
		// This is section for default serialization and parsing - that's why
		// these methods are non-virtual and there're in private section.
		json::Object		_SerializeList(const AppEngine::TaskList& list)
								const;
		json::Object		_SerializeTask(const AppEngine::Task& task) const;
};

} // namespace PluginEngine

#endif // TASKSERIALIZER_H
