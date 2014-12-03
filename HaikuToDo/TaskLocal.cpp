#include "TaskLocal.hpp"
#include <InterfaceKit.h>
#include "Task.hpp"

const char* init_sentence=""
	"CREATE TABLE IF NOT EXISTS HaikuToDo("
	"TITLE TEXT UNIQUE,"
	"DESCRIPTION TEXT,"
	"FINISHED INTEGER);";
const char* load_tasks=""
	"SELECT * FROM HaikuToDo;";
	
TaskLocal::TaskLocal()
{
	int result;
	BPath path;
	error=new BAlert("SQLITE ERROR",
		"There was a SQLite error, an elf might touched the code!",
		"OK", NULL, NULL,
		B_WIDTH_AS_USUAL, B_OFFSET_SPACING, B_STOP_ALERT
	);
	if(find_directory(B_USER_SETTINGS_DIRECTORY,&path)==B_OK)
	{
		path.Append("HaikuToDo.db");
		std::cout << "Path is" << path.Path() << std::endl;
		result=sqlite3_open(path.Path(),&db);
		if(result!=SQLITE_OK)
		{
			error->Go();
			exit(2);
		}
		result=sqlite3_exec(db,init_sentence,0,0,0);
		if(result!=SQLITE_OK)
		{
			error->Go();
			exit(2);
		}
	}else{
		error->Go();
		exit(2);
	}
}

TaskLocal::~TaskLocal()
{
	sqlite3_close(db);
}

void
TaskLocal::LoadTasks(BListView* tasks)
{
	sqlite3_stmt* next;
	if(sqlite3_prepare(db,load_tasks,strlen(load_tasks),&next,0)==SQLITE_OK)
	{
		while(sqlite3_step(next)==SQLITE_ROW)
		{
			const char* title=reinterpret_cast<const char*>(sqlite3_column_text(next,0));
			const char* description=reinterpret_cast<const char*>(sqlite3_column_text(next,1));
			bool finished=(bool)sqlite3_column_int(next,2);
			tasks->AddItem(new Task(title,description,finished));
		}
	}else{
		error->Go();
	}
}

bool
TaskLocal::AddTask(const char* title, const char* description)
{
	BString insert_sentence("INSERT INTO HaikuToDo VALUES(\"");
	insert_sentence.Append(title);
	insert_sentence.Append("\",\"");
	insert_sentence.Append(description);
	insert_sentence.Append("\",0);");
	
	char* error;
	
	if(sqlite3_exec(db,insert_sentence.String(),0,0,&error)!=SQLITE_OK)
	{
		std::cerr << "ERROR: " << error << std::endl;
		return false;
	}else{
		return true;
	}
	
}

bool
TaskLocal::RemoveTask(const char* title, const char* description)
{
	BString remove_sentence("DELETE FROM HaikuToDo WHERE TITLE=\"");
	remove_sentence.Append(title);
	remove_sentence.Append("\";");
	
	char* error;
	
	if(sqlite3_exec(db,remove_sentence.String(),0,0,&error)==SQLITE_OK)
	{
		return true;
	}else{
		std::cerr << "ERROR: " << error << std::endl;
		return false;
	}
}

bool
TaskLocal::MarkAsComplete(const char* title, const char* description)
{
	BString update_sentence("UPDATE HaikuToDo SET FINISHED=1 WHERE TITLE=\"");
	update_sentence.Append(title);
	update_sentence.Append("\";");
	
	char* error;
	
	if(sqlite3_exec(db,update_sentence.String(),0,0,&error)==SQLITE_OK)
	{
		return true;
	}else{
		std::cerr << "ERROR: " << error << std::endl;
	}
}
