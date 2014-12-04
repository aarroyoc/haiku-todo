#include "MainWindow.hpp"
#include "Task.hpp"
#include "AddTask.hpp"

#include <Layout.h>
#include <LayoutBuilder.h>
#include <LayoutItem.h>

MainWindow::MainWindow() 
	: BWindow(BRect(50,50,600+50,400+50),"HaikuToDo",B_TITLED_WINDOW,0)
{
	
	manager=new TaskLocal();
	BView* main=new BView(Bounds(),"Main View",B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
	main->SetViewColor(220,220,220);
	//BGroupLayout* grid=new BGroupLayout(B_HORIZONTAL);
	BGridLayout* grid=new BGridLayout();
	main->SetLayout(grid);
	
	/* Categories View */
	BGridLayout* categoriesLayout=new BGridLayout();
	grid->AddItem(categoriesLayout,0,0);
	//categoriesLayout->SetExplicitAlignment(BAlignment(B_ALIGN_LEFT,B_ALIGN_VERTICAL_CENTER));
	
	taskAdd=new BButton("Add task","Add task",new BMessage(TASK_ADD));
	categoriesLayout->AddView(taskAdd,1,1,2,1);
	
	//taskRemove=new BButton("Remove task","Remove task",new BMessage(TASK_REMOVE));
	//categoriesLayout->AddView(taskRemove,5,1,2,1);
	
	categories=new BListView("Categories list",B_SINGLE_SELECTION_LIST);
	
	BScrollView* scrollCategories=new BScrollView("Scroll categories"
		,categories, 0, false, true);
	categoriesLayout->AddView(scrollCategories,1,3,6,5);
	
	categories->AddItem(new BStringItem("ALL"));
	
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
	BGridLayout* taskLayout=new BGridLayout();
	grid->AddItem(taskLayout,2,0);
	
	taskTitle=new BStringView("Task title","No task selected");
	
	BFont font;
	taskTitle->GetFont(&font);
	font.SetSize(30.0f);
	taskTitle->SetFont(&font,B_FONT_SIZE | B_FONT_FLAGS);
	
	taskLayout->AddView(taskTitle,1,1,6,2);
	
	taskDescription=new BStringView("Task description","No task selected");
	taskLayout->AddView(taskDescription,1,4,5,3);
	
	taskCompleted=new BCheckBox("Task completed","Finished",new BMessage(COMPLETED_TASK));
	taskLayout->AddView(taskCompleted,1,8,5,1);
	
	
	taskLayout->SetInsets(10.0f,10.0f,10.0f,10.0f);
	
	grid->SetMaxColumnWidth(0,300.0f);
	grid->SetMaxColumnWidth(1,300.0f);
	grid->SetMinColumnWidth(2,300.0f);
	
	AddChild(main);
	
	manager->LoadTasks(tasklist);
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
				manager->MarkAsComplete(item->Text(),item->GetDetails());
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
		case RELOAD:
		{
			std::cout << "Reload started" << std::endl;
			tasklist->MakeEmpty();
			manager->LoadTasks(tasklist);
			break;
		}
		case TASK_REMOVE:
		{
			int32 taskSelection=tasklist->CurrentSelection();
			if(taskSelection>=0)
			{
				Task* item=dynamic_cast<Task*>(tasklist->ItemAt(taskSelection));
				manager->RemoveTask(item->Text(),item->GetDetails());
				PostMessage(new BMessage(RELOAD));
			}
			break;
		}
		default:
			BWindow::MessageReceived(msg);
	}
}
