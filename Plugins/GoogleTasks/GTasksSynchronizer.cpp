#define PLUGIN_CLASS_NAME	GTasksSynchronizer;

#include "GTasksSynchronizer.h"
#include "Internet.h"
#include "TaskListManager.h"
#include "TimeRfc3339.h"
#include "Utility.h"

#include <stdexcept>

#include <curl/curl.h>

#include "json.h"


namespace Plugin {

//////////////////////////////////////////////////////////////////////////////
//
//	Constructor & initialization
//
//////////////////////////////////////////////////////////////////////////////


GTasksSynchronizer::GTasksSynchronizer(
	bool synchronizationEnabled,
	int32 synchronizationTimestampSeconds)
	:
	TaskSynchronizer(synchronizationEnabled, synchronizationTimestampSeconds),
	kRequestUrlBase("https://www.googleapis.com/tasks/v1/")
{
	_AuthorizationInit();
}


void
GTasksSynchronizer::_AuthorizationInit()
{
	fAuthorization.fClientId=
		"361523124595-pjc55ituuoabcsg1v0c15kjs1i2vn27b.apps.googleusercontent.com";
	fAuthorization.fScope =				"https://www.googleapis.com/auth/tasks";
	fAuthorization.fClientSecret =		"Ze-ore4EbXZpVrScPCbDKJrg";
	fAuthorization.fAuthorizationUrl =	"https://accounts.google.com/o/oauth2/auth";
	fAuthorization.fTokenUrl =			"https://accounts.google.com/o/oauth2/token";
	
	fAuthorization.CallInternetBrowserForCode();
}


bool
GTasksSynchronizer::ProvideExtraData(void* extraData)
{
	const char* code = reinterpret_cast<const char*>(extraData);
	fAuthorization.fAuthorizationCode = code;
	
	return fAuthorization.GetAccessToken() != "";
}


//////////////////////////////////////////////////////////////////////////////
//
//	Synchronization methods
//
//////////////////////////////////////////////////////////////////////////////


bool
GTasksSynchronizer::_OnSynchronizeAll()
{
	// Is this condition checked in based class?
	if (fManager == nullptr)
		return false;
	
	bool res;
	res = _UpdateCreatedAndDeletedListsAndTasks();
	if (res == false)
		return false;
	
	res = _UpdateExistingListsAndTasks();
	if (res == false)
		return false;
	
	
	fManager->ClearDeletedLists();
	return true;
}


bool
GTasksSynchronizer::_OnSynchronizeList(AppEngine::TaskList& list)
{
	bool created = list.fLastUpdate != 0;
	bool res;
	if (created)
		res = _PushCreatedList(list);
	else
		res = _PushExistingList(list);
	
	if (res == true) {
		list.fLastUpdate = Internet::GetCachedUtcTime();
	}
	return res;
}


bool
GTasksSynchronizer::_OnSynchronizeTask(AppEngine::Task& task)
{
	bool created = task.fLastUpdate != 0;
	bool res;
	if (created)
		res = _PushCreatedTask(task);
	else
		res = _PushExistingTask(task);
	
	if (res == true) {
		task.fLastUpdate = Internet::GetCachedUtcTime();
		task.fLastUpdateCopy.reset(new AppEngine::Task(task));
	}
	return res;
}


//////////////////////////////////////////////////////////////////////////////
//
//	Private helper methods
//
//////////////////////////////////////////////////////////////////////////////


std::vector<BString>
GTasksSynchronizer::_GetRequestHeaders(bool hasJsonBody)
{
	std::vector<BString> headers;
	BString authStr = "Authorization: " + fAuthorization.kTokenType + ' '
		+ fAuthorization.GetAccessToken();
	headers.push_back(authStr);
	
	if (hasJsonBody)
		headers.push_back("Content-Type: application/json");
	
	return headers; 	
}


//////////////////////////////////////////////////////////////////////////////
//
//	Private synchronization helper methods
//
//////////////////////////////////////////////////////////////////////////////


bool
GTasksSynchronizer::_PushCreatedTask(AppEngine::Task& task)
{
	json::Object taskJson;
	taskJson["title"] = task.fTitle.String();
	taskJson["notes"] = task.fNotes.String();
	taskJson["due"] = static_cast<int>(task.fDueDate);
	taskJson["completed"] = task.fCompleted;
	if (task.fParentId != "")
		taskJson["parent"] = task.fParentId.String();
	if (task.fPreviousSiblingId != "")
		taskJson["parent"] = task.fPreviousSiblingId.String();
	
	BString url = kRequestUrlBase + "lists/" + task.fOwner.fId + "/tasks";
	BString response;
	try {
		response = Internet::SendHttpRequest(Internet::HttpMethod::POST, url,
			_GetRequestHeaders(true), json::Serialize(taskJson).c_str());
	}
	catch (std::runtime_error& e) {
		return false;
	}
	std::string responseStdStr = response.String();
	taskJson = json::Deserialize(responseStdStr);
	if (taskJson["id"].GetType() == json::NULLVal)
		return false;
	
	task._ChangeId(static_cast<std::string>(taskJson["id"]).c_str());
	return true;
}


#define CHECK_DIFF(obj, field, jsonObj, jsonField, diffVar) \
	if (obj.fLastUpdateCopy->field != obj.field) { \
		jsonObj[#jsonField] = obj.field; \
		diffVar = true; \
	}

bool
GTasksSynchronizer::_PushExistingTask(AppEngine::Task& task)
{
	json::Object taskJson;
	bool diffValue = false;

	//	if (task.fLastUpdateCopy->fTitle != task.fTitle) {
	//		taskJson["title"] = task.fTitle;
	//		diffValue = true;	
	//	}
	CHECK_DIFF(task, fTitle.String(), taskJson, title, diffValue);
	CHECK_DIFF(task, fNotes.String(), taskJson, notes, diffValue);
	CHECK_DIFF((int)task, fDueDate, taskJson, due, diffValue);
	//CHECK_DIFF(task, fCompleted, taskJson, status, diffValue);
	CHECK_DIFF(task, fDeleted, taskJson, deleted, diffValue);
	
	if (task.fLastUpdateCopy->fCompleted != task.fCompleted) {
		taskJson["status"] = task.fCompleted ? "completed" : "needsAction";
		diffValue = true;	
	}
	
	if (diffValue) {
		taskJson["id"] = task.fId.String();
		BString url = kRequestUrlBase + "lists/" + task.fOwner.fId + "/tasks/"
			+ task.fId;
		BString response;
		try {
			response = Internet::SendHttpRequest(Internet::HttpMethod::PUT, url,
				_GetRequestHeaders(true), json::Serialize(taskJson).c_str());
		}
		catch (std::runtime_error& e) {
			return false;
		}
		std::string responseStdStr = response.String();
		taskJson = json::Deserialize(responseStdStr);
		if (taskJson["id"].GetType() == json::NULLVal)
			return false;
		
		task.fTitle =			static_cast<std::string>(taskJson["title"]).c_str();
		task.fNotes =			static_cast<std::string>(taskJson["notes"]).c_str();
		task.fDueDate =			TimeRfc3339(static_cast<std::string>(
									taskJson["due"]).c_str()).GetUnixUtcTime();
		task.fCompleted =		static_cast<std::string>(taskJson["status"]) == "completed";
		task.fDeleted =			static_cast<bool>(		 taskJson["deleted"]);
	}
	
	if (task.fLastUpdateCopy->fParentId != task.fParentId ||
		task.fLastUpdateCopy->fPreviousSiblingId != task.fPreviousSiblingId) {
		//clear object
		taskJson = json::Object();
		taskJson["id"] = task.fId.String();
		taskJson["parent"] = task.fParentId.String();
		taskJson["previous"] = task.fPreviousSiblingId.String();
		BString url = kRequestUrlBase + "lists/" + task.fOwner.fId + "/tasks/"
			+ task.fId + "/move";
		BString response;
		try {
			response = Internet::SendHttpRequest(Internet::HttpMethod::POST, url,
				_GetRequestHeaders(true), json::Serialize(taskJson).c_str());
		}
		catch (std::runtime_error& e) {
			return false;
		}
		std::string responseStdStr = response.String();
		taskJson = json::Deserialize(responseStdStr);
		if (taskJson["id"].GetType() == json::NULLVal)
			return false;
	}
	
	return true;
}



bool
GTasksSynchronizer::_PushCreatedList(AppEngine::TaskList& list)
{
	json::Object listJson;
	listJson["title"] = list.fTitle.String();
	
	BString url = kRequestUrlBase + "users/@me/lists";
	BString response;
	try {
		response = Internet::SendHttpRequest(Internet::HttpMethod::POST, url,
			_GetRequestHeaders(true), json::Serialize(listJson).c_str());
	}
	catch (std::runtime_error& e) {
		return false;
	}
	std::string responseStdStr = response.String();
	listJson = json::Deserialize(responseStdStr);
	if (listJson["id"].GetType() == json::NULLVal)
		return false;
	
	list._ChangeId(static_cast<std::string>(listJson["id"]).c_str());
	return true;
}


bool
GTasksSynchronizer::_PushExistingList(AppEngine::TaskList& list)
{
	// Don't make diff - simply send. There're little lists
	// and every has only two attribures - title and deleted.
	
	json::Object listJson;
	listJson["id"] = list.fId.String();
	listJson["title"] = list.fTitle.String();
	listJson["deleted"] = list.fDeleted;
	BString url = kRequestUrlBase + "users/@me/lists/" + list.fId;
	BString response;
	try {
		response = Internet::SendHttpRequest(Internet::HttpMethod::PUT, url,
			_GetRequestHeaders(true), json::Serialize(listJson).c_str());
	}
	catch (std::runtime_error& e) {
		return false;
	}
	std::string responseStdStr = response.String();
	listJson = json::Deserialize(responseStdStr);
	if (listJson["id"].GetType() == json::NULLVal)
		return false;
	
	list.fTitle =			static_cast<std::string>(listJson["title"]).c_str();
	list.fDeleted =			static_cast<bool>(		 listJson["deleted"]);
	
	return true;
}


bool
GTasksSynchronizer::_UpdateCreatedAndDeletedListsAndTasks()
{
	for (AppEngine::TaskListManager::TaskListContainer::iterator it =
		fManager->fTaskLists.begin(); it != fManager->fTaskLists.begin(); )
	{
		bool deleted = false;
		
		if ((*it)->fLastUpdate == 0) {
			if (_OnSynchronizeList(**it) == false)
				return false;
		}
		
		else if ((*it)->fDeleted == true) {
			if (_OnSynchronizeList(**it) == false)
				return false;
			it = fManager->fTaskLists.erase(it);
			deleted = true;
		}
		
		if (deleted == false) {
			// FIXME! Refactorization: extract following code to new method!
			AppEngine::TaskList& list = **it;
			for (AppEngine::TaskList::TaskContainer::iterator it2 =
				list.fTaskList.begin(); it2 != list.fTaskList.end(); it2++) {
				bool deleted = false;
				if ((*it2)->fLastUpdate == 0) {
					if (_OnSynchronizeTask(**it2) == false)
						return false;
				}
				
				else if ((*it2)->fDeleted == true) {
					if (_OnSynchronizeTask(**it2) == false)
						return false;
					it2 = list.fTaskList.erase(it2);
					deleted = true;
				}
				if (deleted == false)
					it2++;
			}	
		}
		
		
		if (deleted == false)
			it++;
	}
	return true;
}


bool
GTasksSynchronizer::_UpdateExistingListsAndTasks()
{
	try {
		BString url = kRequestUrlBase + "users/@me/lists";
		std::string lists = Internet::SendHttpRequest(Internet::HttpMethod::GET, url).String();
		json::Object listsJson = json::Deserialize(lists);
		if (listsJson["items"].GetType() == json::NULLVal)
			return false;
		time_t synchronizationTime = Internet::GetCachedUtcTime();
		
		for(json::Object listJson : static_cast<json::Array>(listsJson["items"]))
		{
			BString listId = static_cast<std::string>(listJson["id"]).c_str();
			BString listTitle = static_cast<std::string>(listJson["title"]).c_str();
			bool createdOnServer = AppEngine::TaskList::CheckId(listId) == false;
			
			if (createdOnServer == true) {
				AppEngine::TaskList* list = fManager->AddList(listTitle);
				list->_ChangeId(listId);
			}
			else {
				time_t serverUpdateTime = TimeRfc3339(static_cast<std::string>(
					listJson["updated"]).c_str()).GetUnixUtcTime();
				AppEngine::TaskList& list = *AppEngine::TaskList::GetById(listId);
				
				if (serverUpdateTime > list.fLastUpdate)
					list.fTitle = listTitle;
				else
					if (_OnSynchronizeList(list) == false)
						return false;
			}
			
			AppEngine::TaskList& list = *AppEngine::TaskList::GetById(listId);
			list.fLastUpdate = synchronizationTime;
			
			// update tasks on this list
			if (_UpdateAllExistingTasksOnList(list) == false)
				return false;
		}
		
		// Remove lists which was deleted on server
		for (AppEngine::TaskListManager::TaskListContainer::iterator it =
			fManager->fTaskLists.begin(); it != fManager->fTaskLists.end(); )
		{
			bool deleted = false;
		
			if ((*it)->fLastUpdate < synchronizationTime) {
				it = fManager->fTaskLists.erase(it);
				deleted = true;
			}
			
			if (deleted == false)
				it++;
		}
		
	}
	catch (std::runtime_error& e) {
		return false;	
	}
	
	return true;
}


bool
GTasksSynchronizer::_UpdateAllExistingTasksOnList(AppEngine::TaskList& list)
{
	try {
		BString url = kRequestUrlBase + "lists/" + list.fId + "/tasks";
		std::string tasks = Internet::SendHttpRequest(Internet::HttpMethod::GET, url).String();
		json::Object tasksJson = json::Deserialize(tasks);
		if (tasksJson["items"].GetType() == json::NULLVal)
			return false;
		time_t synchronizationTime = Internet::GetCachedUtcTime();
		
		for(json::Object taskJson : static_cast<json::Array>(tasksJson["items"]))
		{
			BString taskId = static_cast<std::string>(taskJson["id"]).c_str();
			BString taskTitle = static_cast<std::string>(taskJson["title"]).c_str();
			BString taskNotes = static_cast<std::string>(taskJson["notes"]).c_str();
			time_t taskDueDate = TimeRfc3339(static_cast<std::string>(
									taskJson["due"]).c_str()).GetUnixUtcTime();
			bool taskCompleted = static_cast<std::string>(taskJson["status"]) == "completed";
			bool taskDeleted = static_cast<bool>(		 taskJson["deleted"]);
			BString taskParentId = static_cast<std::string>(taskJson["parent"]).c_str();
			// TODO: get position number and find adequate previous task!
			
			bool createdOnServer = AppEngine::Task::CheckId(taskId) == false;
			
			if (createdOnServer == true) {
				AppEngine::Task* task = list.AddTask(nullptr, taskTitle, taskNotes,
					taskDueDate);
				task->_ChangeId(taskId);
				task->fCompleted = taskCompleted;
				task->fDeleted = taskDeleted;
				if (task->fParentId != taskParentId)
					task->SetParent(AppEngine::Task::GetById(taskParentId));
					// FIXME! Can assert - when given parent is not processed yet.
			}
			else {
				time_t serverUpdateTime = TimeRfc3339(static_cast<std::string>(
					taskJson["updated"]).c_str()).GetUnixUtcTime();
				AppEngine::Task& task = *AppEngine::Task::GetById(taskId);
				
				if (serverUpdateTime > list.fLastUpdate) {
					task.fTitle = taskTitle;
					task.fNotes = taskNotes;
					task.fDueDate = taskDueDate;
					task.fCompleted = taskCompleted;
					task.fDeleted = taskDeleted;
					if (task.fParentId != taskParentId)
						task.SetParent(AppEngine::Task::GetById(taskParentId));
						// FIXME! Can assert - when given parent is not processed yet.
				}
				else
					if (_OnSynchronizeTask(task) == false)
						return false;
			}
			
			AppEngine::Task& task = *AppEngine::Task::GetById(taskId);
			task.fLastUpdate = synchronizationTime;
			task.fLastUpdateCopy.reset(new AppEngine::Task(task));
		}
		
		// Remove lists which was deleted on server
		for (AppEngine::TaskList::TaskContainer::iterator it =
			list.fTaskList.begin(); it != list.fTaskList.end(); )
		{
			bool deleted = false;
		
			if ((*it)->fLastUpdate < synchronizationTime) {
				it = list.fTaskList.erase(it);
				deleted = true;
			}
			
			if (deleted == false)
				it++;
		}
		
	}
	catch (std::runtime_error& e) {
		return false;	
	}
	
	return true;
}


} // namespace Plugin
