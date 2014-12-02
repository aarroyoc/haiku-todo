#include "MainWindow.hpp"
#include "TaskListManager.h"

MainWindow::MainWindow() 
	: BWindow(BRect(50,50,450,450),"HaikuToDo",B_TITLED_WINDOW,0)
{
	tasks=new BView(BRect(0,0,100,400),"Tasks",B_FOLLOW_LEFT,B_WILL_DRAW);
	taskdetail=new BView(BRect(100,0,300,300),"Task Detail",B_FOLLOW_RIGHT,B_WILL_DRAW);
	
	tasklist=new BListView(BRect(0,0,100,400),"Task List",B_SINGLE_SELECTION_LIST);
	tasklist->AddItem(new BStringItem("Test Task #1"));
	tasklist->AddItem(new BStringItem("Test Task #2"));
	tasklist->SetSelectionMessage(new BMessage(ITEM_SELECTED));
	tasks->AddChild(new BScrollView("scroll_list",tasklist,B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, false, true));
	
	AddChild(tasks);
	AddChild(taskdetail);
	//manager=new AppEngine::TaskListManager(AppEngine::TaskListManager::InitType::PluginFilename,"/boot/home/todo-list-app/build/libGoogleTasks.so");
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
			printf("Item selected");
		default:
			BWindow::MessageReceived(msg);
	}
}
