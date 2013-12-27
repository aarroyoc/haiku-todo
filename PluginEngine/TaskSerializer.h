#ifndef TASKSERIALIZER_H
#define TASKSERIALIZER_H


#include <String.h>


namespace AppEngine {

class TaskListManager;
	
} // namespace AppEngine


namespace PluginEngine {

class TaskSerializer {
	public:
		virtual BString Serialize(const AppEngine::TaskListManager& tasks)
			const;
		virtual bool Parse(AppEngine::TaskListManager& tasks) const;
};

} // namespace PluginEngine

#endif // TASKSERIALIZER_H
