#include "TaskListManager.h"
#include "TaskSerializer.h"

#include <stdexcept>

#include <Autolock.h>


namespace PluginEngine {

//////////////////////////////////////////////////////////////////////////////
//
//	Public API
//
//////////////////////////////////////////////////////////////////////////////


BString
TaskSerializer::Serialize(const AppEngine::TaskListManager& manager) const
{
	json::Array listsJson;
	
	for(AppEngine::TaskListManager::TaskListContainer::const_iterator it =
		manager.fTaskLists.begin(); it != manager.fTaskLists.end(); it++)
	{
		listsJson.push_back(_SerializeList(**it));
			// *it - is std::unique_ptr<TaskList>
	}
	
	json::Object dataJson;
	dataJson["serialization_type"] = "default";
	dataJson["data"] = listsJson;
	
	return json::Serialize(dataJson).c_str();
}


bool
TaskSerializer::Parse(AppEngine::TaskListManager& manager, const BString& data)
	const
{
	json::Object dataJson;
	
	try {
		std::string dataStdString(data.String());
		dataJson = json::Deserialize(dataStdString);
	}
	catch (std::runtime_error e) {
		return false;	
	}
	
	if (std::string(dataJson["serialization_type"]) != "default")
		return false;
	
	for (json::Object listJson : json::Array(dataJson["data"])) {
		AppEngine::TaskList* list = manager.AddList();
		if (_ParseList(*list, listJson) == false)
			return false;
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//
//	Default serialization
//
//////////////////////////////////////////////////////////////////////////////


json::Object
TaskSerializer::_SerializeList(const AppEngine::TaskList& list) const
{
	BAutolock guard(list.fMutex);
	json::Array tasksJson;
	
	for(AppEngine::TaskList::TaskContainer::const_iterator it =
		list.fTaskList.begin(); it != list.fTaskList.end(); it++)
	{
		tasksJson.push_back(_SerializeTask(**it));
			// *it - is std::unique_ptr<Task>
	}
	
	json::Object listJson;
	listJson["title"] = 				list.fTitle.String();
	listJson["id"] =					list.fId.String();
	listJson["deleted"] =				list.fDeleted;
	listJson["last_update"] = 			static_cast<int>(list.fLastUpdate);
	listJson["last_local_change"] =		static_cast<int>(list.fLastLocalChange);
	listJson["task_list"] =				tasksJson;
	
	return listJson;
}


json::Object
TaskSerializer::_SerializeTask(const AppEngine::Task& task) const
{
	BAutolock guard(task.fMutex);
	json::Object taskJson;
	
	taskJson["title"] =				task.fTitle.String();
	taskJson["notes"] =				task.fNotes.String();
	taskJson["due_date"] =			static_cast<int>(task.fDueDate);
	taskJson["id"] =				task.fId.String();
	taskJson["completed"] =			task.fCompleted;
	taskJson["deleted"] =			task.fDeleted;
	taskJson["parent"] =			task.fParentId.String();
	taskJson["first_child"] =		task.fFirstChildId.String();
	taskJson["next_sibling"] =		task.fNextSiblingId.String();
	taskJson["previous_sibling"] =	task.fPreviousSiblingId.String();
	taskJson["is_copy_object"] =	task.fIsCopyObject;
	
	if (task.fIsCopyObject == false) {
		taskJson["last_update"] =	static_cast<int>(task.fLastUpdate);
		taskJson["last_local_change"] =
									static_cast<int>(task.fLastLocalChange);
		if (task.fLastUpdateCopy)
			taskJson["last_update_copy"] =
									_SerializeTask(*task.fLastUpdateCopy);
		else
			taskJson["last_update_copy"] =
									json::Value(); // null value
	}
	
	return taskJson;	
}


//////////////////////////////////////////////////////////////////////////////
//
//	Default parsing
//
//////////////////////////////////////////////////////////////////////////////


bool
TaskSerializer::_ParseList(AppEngine::TaskList& list,
	json::Object& listJson) const
{
	std::vector<std::string> keys = {"title", "id", "deleted", "last_update",
		"last_local_change", "task_list"};
	if (_CheckKeys(listJson, keys) == false)
		return false;
	
	list.fTitle =			static_cast<std::string>(listJson["title"]).c_str();
	list._ChangeId(			static_cast<std::string>(listJson["id"]).c_str());
	list.fDeleted =			static_cast<bool>(		 listJson["deleted"]);
	list.fLastUpdate =		static_cast<int>(		 listJson["last_update"]);
	list.fLastLocalChange =	static_cast<int>(		 listJson["last_local_change"]);
	json::Array tasksJson =	static_cast<json::Array>(listJson["task_list"]);
	
	for (json::Object taskJson : tasksJson)
	{
		AppEngine::Task* task = list.AddTask();
		if (_ParseTask(*task, taskJson) == false)
			return false;
	}
	
	return true;
}


bool
TaskSerializer::_ParseTask(AppEngine::Task& task, json::Object& taskJson)
	const
{
	std::vector<std::string> keys = {"is_copy_object", "title", "notes",
		"due_date", "id", "completed", "deleted", "parent", "first_child",
		"next_sibling", "previous_sibling"};
	if (_CheckKeys(taskJson, keys) == false)
		return false;
	
	if (static_cast<bool>(taskJson["is_copy_object"]) == false) {
		std::vector<std::string> keys = {"last_update", "last_local_change"};
		if (_CheckKeys(taskJson, keys) == false)
			return false;
		
		task.fLastUpdate =		static_cast<int>(taskJson["last_update"]);
		task.fLastLocalChange =	static_cast<int>(taskJson["last_local_change"]);
		if (taskJson["last_update_copy"].GetType() != json::NULLVal) {
			json::Object copyTaskJson(taskJson["last_update_copy"]);
			if (_ParseTask(task, copyTaskJson) == false)
				return false;
			AppEngine::Task* copyTask = new AppEngine::Task(task);
			task.fLastUpdateCopy.reset(copyTask);
		}
	}
	
	task.fTitle =			static_cast<std::string>(taskJson["title"]).c_str();
	task.fNotes =			static_cast<std::string>(taskJson["notes"]).c_str();
	task.fDueDate =			static_cast<int>(		 taskJson["due_date"]);
	task._ChangeId(			static_cast<std::string>(taskJson["id"]).c_str());
	task.fCompleted =		static_cast<bool>(		 taskJson["completed"]);
	task.fDeleted =			static_cast<bool>(		 taskJson["deleted"]);
	task.fParentId =		static_cast<std::string>(taskJson["parent"]).c_str();
	task.fFirstChildId =	static_cast<std::string>(taskJson["first_child"]).c_str();
	task.fNextSiblingId =	static_cast<std::string>(taskJson["next_sibling"]).c_str();
	task.fPreviousSiblingId =static_cast<std::string>(taskJson["previous_sibling"]).c_str();
	
	return true;	
}


//////////////////////////////////////////////////////////////////////////////
//
//	Other methods
//
//////////////////////////////////////////////////////////////////////////////


bool
TaskSerializer::_CheckKeys(json::Object& object,
	const std::vector<std::string>& keys) const
{
	for (auto& key : keys)
		if (object[key].GetType() == json::NULLVal)
			return false;
	return true;
}


} // namespace PluginEngine
