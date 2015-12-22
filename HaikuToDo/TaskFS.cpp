#include "TaskFS.hpp"
#include <iostream>
#include "Task.hpp"
#include "Category.hpp"
#include <cstdlib>
#include <TypeConstants.h>
#include <posix/sys/stat.h>
#include <kernel/fs_index.h>

TaskFS::TaskFS()
{
	error=new BAlert("FileSystem error",
		"There was an error in the BeFS backend",
		"OK",NULL,NULL,B_WIDTH_AS_USUAL,B_OFFSET_SPACING, B_STOP_ALERT);
	BPath path;
	if(find_directory(B_USER_SETTINGS_DIRECTORY,&path)!=B_OK)
	{
		error->Go();
		exit(2);
	}
	BDirectory storage;
	BDirectory tasksDir;
	BDirectory categoriesDir;
	BDirectory settings(path.Path());
	settings.CreateDirectory("HaikuToDo",&storage);
	storage.CreateDirectory("Tasks",&tasksDir);
	storage.CreateDirectory("Categories",&categoriesDir);
	path.Append("HaikuToDo");
	BPath taskPath=path;
	taskPath.Append("Tasks");
	BPath categoriesPath=path;
	categoriesPath.Append("Categories");
	tasks=BString(taskPath.Path());
	categories=BString(categoriesPath.Path());

	struct stat st;
	settings.GetStat(&st);

	volume=st.st_dev;

	fs_create_index(st.st_dev,"HAIKU_TO_DO:Category",B_STRING_TYPE,0);

}

TaskFS::~TaskFS()
{

}

void
TaskFS::LoadTasks(const char* category, BListView* tasksList)
{
	BString predicate("HAIKU_TO_DO:Category=**");
	if(strcmp(category,"ALL")==0)
		predicate.Append("**");
	else
		predicate.Append(category);
	BQuery query;
	BVolume volume;
	BVolumeRoster volumeRoster;
	while(volumeRoster.GetNextVolume(&volume)==B_OK)
	{
		if(volume.KnowsQuery())
		{
			query.Clear();
			query.SetVolume(&volume);
			query.SetPredicate(predicate.String());
			status_t rc=query.Fetch();
			if(rc!=B_OK)
			{
				error->Go();
			}
			BEntry entry;
			while(query.GetNextEntry(&entry)==B_OK)
			{
				char name[8192];
				entry.GetName(name);
				BFile file(&entry,B_READ_ONLY);
				off_t file_size=0;
				file.GetSize(&file_size);
				char buffer[file_size+1024];
				file.Read(buffer,file_size);
				bool finished;
				file.ReadAttr("HAIKU_TO_DO:Finished",B_BOOL_TYPE,0,&finished,sizeof(bool));
				Task* tk=new Task((const char*)name,(const char*)buffer,"ALL",finished);
				tasksList->AddItem(tk);
			}
		}
	}

}

void
TaskFS::LoadCategories(BListView* categoriesList)
{
		BDirectory dir(categories);
		int32 entries=dir.CountEntries();
		for(int32 i=0;i<entries;i++)
		{
			char name[8192];
			BEntry entry;
			dir.GetNextEntry(&entry);
			entry.GetName(name);
			BFile file(&entry,B_READ_ONLY);
			off_t file_size=0;
			file.GetSize(&file_size);
			char buffer[file_size+1024];
			file.Read(buffer,file_size);
			Category* cat=new Category((const char*)name,(const char*)buffer);
			categoriesList->AddItem(cat);
		}
}

bool
TaskFS::AddCategory(const char* name, const char* filename)
{
	BString filn(filename);
	BDirectory dir(categories);
	BFile file;
	dir.CreateFile(name,&file);
	file.Write(filn,filn.Length());
}

bool
TaskFS::AddTask(const char* title, const char* description, const char* category)
{
	BString desc(description);
	BDirectory dir(tasks);
	BFile file;
	dir.CreateFile(title,&file);
	file.Write(desc,desc.Length());
	file.WriteAttrString("HAIKU_TO_DO:Category",new BString(category));
	bool finished=false;
	file.WriteAttr("HAIKU_TO_DO:Finished",B_BOOL_TYPE,0,&finished,sizeof(bool));
}

bool
TaskFS::RemoveTask(const char* title, const char* description, const char* category)
{
	BString predicate("(HAIKU_TO_DO:Category=**)&&(name=");
	predicate.Append(title);
	predicate.Append(")");

	BQuery query;
	BVolume volume;
	BVolumeRoster volumeRoster;
	while(volumeRoster.GetNextVolume(&volume)==B_OK)
	{
		if(volume.KnowsQuery())
		{
			query.Clear();
			query.SetVolume(&volume);
			query.SetPredicate(predicate.String());
			status_t rc=query.Fetch();
			if(rc!=B_OK)
			{
				error->Go();
			}
			BEntry entry;
			while(query.GetNextEntry(&entry)==B_OK)
			{
				entry.Remove();
			}
		}
	}
}

bool
TaskFS::MarkAsComplete(const char* title, const char* description, const char* category)
{
	BString predicate("(HAIKU_TO_DO:Category=**)&&(name=");
	predicate.Append(title);
	predicate.Append(")");

	BQuery query;
	BVolume volume;
	BVolumeRoster volumeRoster;
	while(volumeRoster.GetNextVolume(&volume)==B_OK)
	{
		if(volume.KnowsQuery())
		{
			query.Clear();
			query.SetVolume(&volume);
			query.SetPredicate(predicate.String());
			status_t rc=query.Fetch();
			if(rc!=B_OK)
			{
				error->Go();
			}
			BEntry entry;
			while(query.GetNextEntry(&entry)==B_OK)
			{
				BFile file(&entry,B_READ_ONLY);
				bool finished=true;
				file.WriteAttr("HAIKU_TO_DO:Finished",B_BOOL_TYPE,0,&finished,sizeof(bool));
			}
		}
	}
}

