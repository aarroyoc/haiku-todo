#include "MainWindow.hpp"
#include "Task.hpp"
#include "AddTask.hpp"

MainWindow::MainWindow() 
	: BWindow(BRect(50,50,450,450),"HaikuToDo",B_TITLED_WINDOW,0)
{
	
	manager=new TaskLocal();
	
	tasks=new BView(BRect(0,0,100,400),"Tasks",B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	taskdetail=new BView(BRect(100,0,400,400),"Task Details",B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	
	tasklist=new BListView(BRect(0,0,100,400),"Task List",B_SINGLE_SELECTION_LIST);
	manager->LoadTasks(tasklist);
	tasklist->SetSelectionMessage(new BMessage(ITEM_SELECTED));
	tasks->AddChild(new BScrollView("scroll_list",tasklist,
		B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, false, true));
	
	
	taskdetail->SetViewColor(220,220,220);
	
	BFont font;
	taskdetail->GetFont(&font);
	font.SetSize(32.0);
	
	taskTitle=new BStringView(BRect(20,0,400,100),"Task Title",
		"No task selected",B_FOLLOW_RIGHT | B_FOLLOW_TOP);
	taskTitle->SetFont(&font,B_FONT_SIZE | B_FONT_FLAGS);
	taskDescription=new BStringView(BRect(20,50,400,200),"Task Description",
		"No task selected");
	taskCompleted=new BCheckBox(BRect(20,250,100,50),"Task Completed",
		"Finished",new BMessage(COMPLETED_TASK));
	taskAdd=new BButton(BRect(20,350,100,75),"Add Task","Add task",
		new BMessage(TASK_ADD));
	taskRemove=new BButton(BRect(150,350,250,75),"Remove task","Remove task",
		new BMessage(TASK_REMOVE));
	
	taskdetail->AddChild(taskTitle);
	taskdetail->AddChild(taskDescription);
	taskdetail->AddChild(taskCompleted);
	taskdetail->AddChild(taskAdd);
	taskdetail->AddChild(taskRemove);
	
	AddChild(tasks);
	AddChild(taskdetail);
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
				 
			}
			break;
		}
		case TASK_ADD:
		{
			AddTask* add=new AddTask(manager);
			add->Show();
			
			break;
		}
		case RELOAD:
		{
			std::cout << "Reload started" << std::endl;
			tasklist->MakeEmpty();
			manager->LoadTasks(tasklist);
			break;
		}
		case TASK_REMOVE:
		{
			break;
		}
		default:
			BWindow::MessageReceived(msg);
	}
}
