#include "TaskListManager.h"
#include "TaskSerializer.h"

#include "json.h"


namespace PluginEngine {

//////////////////////////////////////////////////////////////////////////////
//
//	Public API
//
//////////////////////////////////////////////////////////////////////////////


BString
TaskSerializer::Serialize(const AppEngine::TaskListManager& manager) const
{
	json::Array lists;
	
	for(AppEngine::TaskListManager::TaskListContainer::const_iterator it =
		manager.fTaskLists.begin(); it != manager.fTaskLists.end(); it++)
	{
		lists.push_back(_SerializeList(**it));
			// *it - is std::unique_ptr<TaskList>
	}
	
	json::Object data;
	data["serialization_type"] = "default";
	data["data"] = lists;
	
	return json::Serialize(data).c_str();
}




//////////////////////////////////////////////////////////////////////////////
//
//	Default serialization and parsing
//
//////////////////////////////////////////////////////////////////////////////


json::Object
TaskSerializer::_SerializeList(const AppEngine::TaskList& list) const
{
	json::Array tasks;
	
	for(AppEngine::TaskList::TaskContainer::const_iterator it =
		list.fTaskList.begin(); it != list.fTaskList.end(); it++)
	{
		tasks.push_back(_SerializeTask(**it));
			// *it - is std::unique_ptr<Task>
	}
	
	json::Object listObj;
	listObj["title"] = 				list.fTitle.String();
	listObj["id"] =					list.fId.String();
	listObj["deleted"] =			list.fDeleted;
	listObj["last_update"] = 		static_cast<int>(list.fLastUpdate);
	listObj["last_local_change"] =	static_cast<int>(list.fLastLocalChange);
	listObj["task_list"] =			tasks;
	
	return listObj;
}


json::Object
TaskSerializer::_SerializeTask(const AppEngine::Task& task) const
{
	json::Object taskObj;
	
	taskObj["title"] =				task.fTitle.String();
	taskObj["notes"] =				task.fNotes.String();
	taskObj["due_date"] =			static_cast<int>(task.fDueDate);
	taskObj["id"] =					task.fId.String();
	taskObj["completed"] =			task.fCompleted;
	taskObj["deleted"] =			task.fDeleted;
	taskObj["parent"] =				task.fParentId.String();
	taskObj["first_child"] =		task.fFirstChildId.String();
	taskObj["next_sibling"] =		task.fNextSiblingId.String();
	taskObj["previous_sibling"] =	task.fPreviousSiblingId.String();
	
	if (task.fIsCopyObject == false) {
		taskObj["last_update"] =	static_cast<int>(task.fLastUpdate);
		taskObj["last_local_change"] =
									static_cast<int>(task.fLastLocalChange);
		if (task.fLastUpdateCopy)
			taskObj["last_update_copy"] =
									_SerializeTask(*task.fLastUpdateCopy);
		else
			taskObj["last_update_copy"] =
									json::Value(); // null value
	}
	
	return taskObj;	
}

} // namespace PluginEngine
