#include "MainWindow.hpp"
#include "Task.hpp"
#include "AddTask.hpp"
#include "Category.hpp"
#include "TaskGoogle.hpp"
#include "TaskSync.hpp"

#include <Layout.h>
#include <LayoutBuilder.h>
#include <LayoutItem.h>

#define CATEGORY_ALL_ICON "MIME_DATABASE"

MainWindow::MainWindow()
	: BWindow(BRect(50,50,600+50,400+50),"HaikuToDo",B_TITLED_WINDOW,0)
{
	#ifdef BUILD_SQLITE
	manager=new TaskSQL();
	#else
	manager=new TaskFS();
	#endif
	//DOING TESTS WITH GOOGLE TASKS
	sync=new TaskGoogle();
	//sync->Login();
	//SEND MESSAGE WHEN LOADED
	//END DOING TESTS
	BView* main=new BView(Bounds(),"Main View",B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	main->SetViewColor(220,220,220);
	//BGroupLayout* grid=new BGroupLayout(B_HORIZONTAL);
	BGridLayout* grid=new BGridLayout();
	main->SetLayout(grid);

	/* Categories View */
	BGridLayout* categoriesLayout=new BGridLayout();
	grid->AddItem(categoriesLayout,0,0);

	taskAdd=new BButton("Add task","Add task",new BMessage(TASK_ADD));
	categoriesLayout->AddView(taskAdd,1,1,2,1);

	categories=new BListView("Categories list",B_SINGLE_SELECTION_LIST);
	categories->SetSelectionMessage(new BMessage(CHANGE_CATEGORY));

	BScrollView* scrollCategories=new BScrollView("Scroll categories"
		,categories, 0, false, true);
	categoriesLayout->AddView(scrollCategories,1,3,6,5);

	googleTasks=new BButton("Google Tasks","Google Tasks",new BMessage(LOGIN_GTASKS));
	categoriesLayout->AddView(googleTasks,1,8,2,1);

	categories->AddItem(new Category("ALL",CATEGORY_ALL_ICON));
	manager->LoadCategories(categories);

	categoriesLayout->SetInsets(10.0f,10.0f,10.0f,10.0f);


	/* List View */
	BGridLayout* listLayout=new BGridLayout();
	grid->AddItem(listLayout,1,0);

	taskRemove=new BButton("Remove task","Remove task",new BMessage(TASK_REMOVE));
	listLayout->AddView(taskRemove,1,1,2,1);

	tasklist=new BListView("Tasks list",B_SINGLE_SELECTION_LIST);
	tasklist->SetSelectionMessage(new BMessage(ITEM_SELECTED));

	BScrollView* scrollTasks=new BScrollView("Scroll tasks", tasklist,0
		,false, true);
	listLayout->AddView(scrollTasks,1,3,6,5);
	listLayout->SetInsets(0.0f,10.0f,0.0f, 10.0f);

	/* Task View */
	BGroupLayout* taskLayout=new BGroupLayout(B_VERTICAL);
	grid->AddItem(taskLayout,2,0);

	taskTitle=new BStringView("Task title","No task selected");

	BFont font;
	taskTitle->GetFont(&font);
	font.SetSize(20.0f);
	taskTitle->SetFont(&font,B_FONT_SIZE | B_FONT_FLAGS);

	taskLayout->AddView(taskTitle);//,1,1,6,2);

	taskDescription=new BTextView("Task description");
	taskDescription->MakeEditable(false);
	taskDescription->SetViewColor(220,220,220);
	taskLayout->AddView(taskDescription);//,1,4//,5,3);

	taskCompleted=new BCheckBox("Task completed","Finished",new BMessage(COMPLETED_TASK));
	taskLayout->AddView(taskCompleted);//,1,8,5,1);


	taskLayout->SetInsets(10.0f,10.0f,10.0f,10.0f);

	grid->SetMaxColumnWidth(0,300.0f);
	grid->SetMaxColumnWidth(1,300.0f);
	grid->SetMinColumnWidth(2,300.0f);

	AddChild(main);

	manager->LoadTasks("ALL",tasklist);
}

MainWindow::~MainWindow()
{

}

bool
MainWindow::QuitRequested()
{
	be_app_messenger.SendMessage(B_QUIT_REQUESTED);
	return BWindow::QuitRequested();
}

void
MainWindow::MessageReceived(BMessage* msg)
{
	switch(msg->what)
	{
		case ITEM_SELECTED:
		{
			int32 taskSelection=tasklist->CurrentSelection();
			if(taskSelection>=0)
			{
				Task* item=dynamic_cast<Task*>(tasklist->ItemAt(taskSelection));
				SetTitle(item->Text());
				taskTitle->SetText(item->Text());
				taskDescription->SetText(item->GetDetails());
				taskCompleted->SetValue(item->GetCompleted());
			}
			break;
		}
		case COMPLETED_TASK:
		{
			//ASK FOR REMOVE TASK
			int32 taskSelection=tasklist->CurrentSelection();
			if(taskSelection>=0)
			{
				Task* item=dynamic_cast<Task*>(tasklist->ItemAt(taskSelection));
				manager->MarkAsComplete(item->Text(),item->GetDetails(),item->GetCategory());
				PostMessage(new BMessage(RELOAD));
			}
			break;
		}
		case TASK_ADD:
		{
			AddTask* add=new AddTask(manager);
			add->Show();

			break;
		}
		case SYNC_CATEGORIES:
		{
			BList* cats=sync->GetCategories();
			categories->AddList(cats);
			break;
		}
		case LOGIN_GTASKS:
		{
			std::cout << "Login one time" << std::endl;
			sync->Login();
			break;
		}
		case RELOAD:
		{
			std::cout << "Reload started" << std::endl;
			tasklist->MakeEmpty();
			int32 selection=categories->CurrentSelection();
			if(selection>=0)
			{
				Category* item=dynamic_cast<Category*>(categories->ItemAt(selection));
				if(strcmp(item->GetID(),"NULL")==0)
				{
					manager->LoadTasks(item->GetName(),tasklist);
				}else{
					tasklist->AddList(sync->GetTasks(item));
				}

			}else{
				manager->LoadTasks("ALL",tasklist);
			}
			break;
		}
		case RELOAD_CATEGORIES:
		{
			categories->MakeEmpty();
			categories->AddItem(new Category("ALL",CATEGORY_ALL_ICON));
			manager->LoadCategories(categories);
			break;
		}
		case TASK_REMOVE:
		{
			int32 taskSelection=tasklist->CurrentSelection();
			if(taskSelection>=0)
			{
				Task* item=dynamic_cast<Task*>(tasklist->ItemAt(taskSelection));
				manager->RemoveTask(item->Text(),item->GetDetails(),item->GetCategory());
				PostMessage(new BMessage(RELOAD));
			}
			break;
		}
		case CHANGE_CATEGORY:
		{
			PostMessage(new BMessage(RELOAD));
		}
		default:
			BWindow::MessageReceived(msg);
	}
}
