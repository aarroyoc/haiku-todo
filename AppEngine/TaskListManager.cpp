#include "SynchronizationPlugin.h"
#include "TaskListManager.h"
#include "TaskSerializer.h"
#include "TaskSynchronizer.h"

#include <cassert>

#include "Autolock.h"
#include "File.h"


namespace AppEngine {

//////////////////////////////////////////////////////////////////////////////
//
//	Constructor & destructor
//
//////////////////////////////////////////////////////////////////////////////


TaskListManager::TaskListManager(InitType type, BString data)
	:
	fPluginLoader("")
		// Get default plugin (check _Parse() method for further informations)
{
	switch (type) {
		case InitType::PluginFilename:
			fPluginLoader.Load(data);
			break;
		
		case InitType::ContentFilename:
			_ReadFromFile(data);
			break;
		
		case InitType::SerializedData:
			_Parse(data);
			break;
			
		default:
			assert("TaskListManager::(constructor) - invalid InitType!"
				== nullptr);
			break;
	}
}


//////////////////////////////////////////////////////////////////////////////
//
//	Public methods - getters
//
//////////////////////////////////////////////////////////////////////////////


const TaskListManager::TaskListContainer&
TaskListManager::GetLists() const
{
	return fTaskLists;	
}


BString
TaskListManager::Serialize() const
{
	BString serializedData;
	serializedData << "!plugin_filename:" << fPluginLoader.GetPluginFilename()
		<< "\n";
	serializedData << _GetSerializer().Serialize(*this);
	return serializedData;
}


bool
TaskListManager::SaveToFile(BString filename) const
{
	BFile file;
	status_t openStatus = file.SetTo(filename.String(),
		B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
	if (openStatus != B_OK)
		return false;
	
	BString data = Serialize();
	ssize_t writtenBytes = file.Write(data.String(), data.Length());
		// File is closed in BFile destructor
	return writtenBytes == data.Length();
}


//////////////////////////////////////////////////////////////////////////////
//
//	Public methods - modifiers
//
//////////////////////////////////////////////////////////////////////////////


TaskList*
TaskListManager::AddList(BString title)
{
	TaskList* list = new TaskList(*this, title);
	std::unique_ptr<TaskList> listPtr(list);
	fTaskLists.push_back(std::move(listPtr));
	
	return list;	
}


void
TaskListManager::ClearDeletedLists()
{
	for(TaskListContainer::iterator it = fTaskLists.begin();
		it != fTaskLists.end();)
	{
		bool deletedRecently = false;
		
		BAutolock((*it)->fMutex);
		if ((*it)->fDeleted == true &&
			 ( (*it)->fLastUpdate >= (*it)->fLastLocalChange ||
			 (*it)->fLastUpdate == 0) ) {
				// delete only when task is deleted on server or doesn't exists
			it = fTaskLists.erase(it);
			deletedRecently = true;
		}
		
		if (deletedRecently == false)
			it++;
	}	
}


//////////////////////////////////////////////////////////////////////////////
//
//	Private methods - getters
//
//////////////////////////////////////////////////////////////////////////////


PluginEngine::TaskSynchronizer&
TaskListManager::_GetSynchronizer() const
{
	return fPluginLoader.GetPlugin().GetSynchronizer();	
}


PluginEngine::TaskSerializer&
TaskListManager::_GetSerializer() const
{
	return fPluginLoader.GetPlugin().GetSerializer();	
}


//////////////////////////////////////////////////////////////////////////////
//
//	Private methods - synchronization
//
//////////////////////////////////////////////////////////////////////////////


void
TaskListManager::_OnUpdate(Task& task) const
{
	_GetSynchronizer().SynchronizeTask(task);
}


void
TaskListManager::_OnUpdate(TaskList& list) const
{
	_GetSynchronizer().SynchronizeList(list);
}


//////////////////////////////////////////////////////////////////////////////
//
//	Private methods - modifiers
//
//////////////////////////////////////////////////////////////////////////////


bool
TaskListManager::_Parse(BString serializedData)
{
	int32 firstLineLength = serializedData.FindFirst('\n');
	if (firstLineLength == B_ERROR)
		return false;
	
	BString firstLine, serializedTasks;
	serializedData.CopyInto(firstLine, 0, firstLineLength);
	serializedTasks = serializedData;
	serializedTasks.Remove(0, firstLineLength + 1);
	
	BString pluginName = firstLine;
	BString firstBytes = "!plugin_filename:";
		// see Serialize()
	if (firstLine.Compare(firstBytes, firstBytes.Length()) != 0)
		return false;
	pluginName.Remove(0, firstBytes.Length());
	
	
	fPluginLoader.Unload();
		// some plugin MUST be loaded! (check constructor)
	fTaskLists.clear();
	
	if (fPluginLoader.Load(pluginName) == false) {
		fPluginLoader.Load("");
		return false;
	}
	_GetSynchronizer().SetSynchronizationTarget(this);
	
	if (_GetSerializer().Parse(*this, serializedTasks) == false) {
		fPluginLoader.Unload();
		fPluginLoader.Load("");
		fTaskLists.clear();
		return false;
	}
	
	return true;
}


bool
TaskListManager::_ReadFromFile(BString filename)
{
	BFile file;
	status_t status = file.SetTo(filename.String(), B_READ_ONLY);
	if (status != B_OK)
		return false;
	
	off_t fileSize;
	if (file.GetSize(&fileSize) != B_OK)
		return false;
		
	BString buffer;
	char* bufferPtr = buffer.LockBuffer(fileSize);
		// memory will be released by BString
	ssize_t readBytes = file.Read(bufferPtr, fileSize);
	if (readBytes != fileSize)
		return false;
	
	buffer.UnlockBuffer();
	return _Parse(buffer);
}


} // namespace AppEngine
