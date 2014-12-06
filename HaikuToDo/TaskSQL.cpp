#include "TaskSQL.hpp"
#include <InterfaceKit.h>
#include "Task.hpp"
#include "Category.hpp"

TaskSQL::TaskSQL()
{
	const char* init_sentence=""
	"CREATE TABLE IF NOT EXISTS HaikuToDo("
	"TITLE TEXT UNIQUE,"
	"DESCRIPTION TEXT,"
	"CATEGORY TEXT,"
	"FINISHED INTEGER);";
	const char* init_sentence2=""
	"CREATE TABLE IF NOT EXISTS Categories("
	"NAME TEXT,"
	"PICTURE TEXT);";
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
		std::cout << "Path is " << path.Path() << std::endl;
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
		result=sqlite3_exec(db,init_sentence2,0,0,0);
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

TaskSQL::~TaskSQL()
{
	sqlite3_close(db);
}

void
TaskSQL::LoadTasks(const char* category,BListView* tasks)
{
	BString load_tasks("SELECT * FROM HaikuToDo");
	if(strcmp("ALL",category)==0)
	{
		load_tasks.Append(";");
	}else{
		load_tasks.Append(" WHERE CATEGORY=\"");
		load_tasks.Append(category);
		load_tasks.Append("\";");
	}
	sqlite3_stmt* next;
	if(sqlite3_prepare(db,load_tasks.String(),strlen(load_tasks.String()),&next,0)==SQLITE_OK)
	{
		while(sqlite3_step(next)==SQLITE_ROW)
		{
			const char* title=reinterpret_cast<const char*>(sqlite3_column_text(next,0));
			const char* description=reinterpret_cast<const char*>(sqlite3_column_text(next,1));
			const char* category=reinterpret_cast<const char*>(sqlite3_column_text(next,2));
			bool finished=(bool)sqlite3_column_int(next,3);
			tasks->AddItem(new Task(title,description,category,finished));
		}
	}else{
		error->Go();
	}
}

void
TaskSQL::LoadCategories(BListView* categories)
{
	BString load_categories("SELECT * FROM Categories;");
	
	sqlite3_stmt* next;
	if(sqlite3_prepare(db,load_categories.String(),strlen(load_categories.String()),&next,0)==SQLITE_OK)
	{
		while(sqlite3_step(next)==SQLITE_ROW)
		{
			const char* category=reinterpret_cast<const char*>(sqlite3_column_text(next,0));
			const char* path=reinterpret_cast<const char*>(sqlite3_column_text(next,1));
			categories->AddItem(new Category(category,path));
		}
	}else
	{
		error->Go();
	}
}

bool
TaskSQL::AddCategory(const char* name, const char* filename)
{
	BString insert_sentence("INSERT INTO Categories VALUES(\"");
	insert_sentence.Append(name);
	insert_sentence.Append("\",\"");
	insert_sentence.Append(filename);
	insert_sentence.Append("\");");
	
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
TaskSQL::AddTask(const char* title, const char* description, const char* category)
{
	BString insert_sentence("INSERT INTO HaikuToDo VALUES(\"");
	insert_sentence.Append(title);
	insert_sentence.Append("\",\"");
	insert_sentence.Append(description);
	insert_sentence.Append("\",\"");
	insert_sentence.Append(category);
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
TaskSQL::RemoveTask(const char* title, const char* description, const char* category)
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
TaskSQL::MarkAsComplete(const char* title, const char* description, const char* category)
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


