#ifndef ADDTASK_HPP
#define ADDTASK_HPP

#include <InterfaceKit.h>
#include "TaskLocal.hpp"
#include "MainWindow.hpp"

const int32 SAVE_TASK=10;
const int32 CANCEL=200;

class AddTask : public BWindow{
	public:
		AddTask(TaskLocal* manager) : BWindow(BRect(100,100,400,400),
			"Add a task",B_MODAL_WINDOW,B_NOT_MINIMIZABLE| B_NOT_RESIZABLE),
			manager(manager)
		{
			BView* view=new BView(Bounds(),NULL,B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
			view->SetViewColor(220,220,220);
			AddChild(view);
			//CREATE WINDOW AND BUTTONS
			title=new BTextControl(BRect(15,15,300,75),"Title","Title:","",NULL);
			description=new BTextView(BRect(15,100,275,150),"Description",Bounds(),B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
			BScrollView* scroll=new BScrollView(NULL,description);
			save=new BButton(BRect(15,225,115,300),"Save","Save",new BMessage(SAVE_TASK));
			cancel=new BButton(BRect(150,225,250,300),"Cancel","Cancel",new BMessage(CANCEL));
			
			view->AddChild(title);
			view->AddChild(scroll);
			view->AddChild(save);
			view->AddChild(cancel);
		}
		~AddTask()
		{
			
		}
	private:
		void
		MessageReceived(BMessage* msg)
		{
			switch(msg->what)
			{
				case SAVE_TASK:
				{
					if(!manager->AddTask(title->Text(),description->Text()))
					{
						BAlert* error=new BAlert("SAVING ERROR",
							"Oops, we can't save that in database",
							"OK",NULL,NULL,B_WIDTH_AS_USUAL, B_OFFSET_SPACING,
							 B_STOP_ALERT);
						error->Go();
					}
					//SEND RELOAD MESSAGE
					int32 count=be_app->CountWindows();
					for(int32 i=0;i<count;i++)
					{
						be_app->WindowAt(i)->PostMessage(new BMessage(RELOAD));
					}
				}
				case CANCEL:
				{
					Quit();
				}
				default:
					BWindow::MessageReceived(msg);
			}
		}
		BTextControl*	title;
		BTextView*		description;
		BButton*		save;
		BButton*		cancel;
		TaskLocal* 		manager;
};

#endif
