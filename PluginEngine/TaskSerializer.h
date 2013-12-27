#ifndef TASKSERIALIZER_H
#define TASKSERIALIZER_H


#include <String.h>


namespace PluginEngine {

class TaskSerializer {
	public:
		virtual BString Serialize(const TaskListManager& tasks) const;
		virtual void Parse(TaskListManager& tasks) const;
};

} // namespace PluginEngine

#endif // TASKSERIALIZER_H
