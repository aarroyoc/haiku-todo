#ifndef TASKGOOGLE_HPP
#define TASKGOOGLE_HPP

#include "TaskSync.hpp"
#include <InterfaceKit.h>
#include "MainWindow.hpp"
#include <iostream>
#include <vector>
#include "Category.hpp"
#include "Task.hpp"

class TaskGoogle : public TaskSync{
	public:
									TaskGoogle();
									~TaskGoogle();
		bool						Login();
		void						NextStep(BString code);
		BList*						GetCategories();
		BList*						GetTasks(Category* category);
	private:
		BString token;
		BList* categories;
		BList* tasks;
};

const int32 LOGIN_CODE=700;

class LoginDialog : public BWindow{
	public:
		LoginDialog(TaskSync* auth) : BWindow(BRect(100,100,300,300),"Login code",B_MODAL_WINDOW,0)
			, auth(auth)
		{
			BView* main=new BView(Bounds(),NULL,B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
			main->SetViewColor(220,220,220);
			
			code=new BTextControl(BRect(10,10,200,100),"Code","Code: ","",NULL);
			main->AddChild(code);
			
			login=new BButton(BRect(10,110,200,200),NULL,"Login",new BMessage(LOGIN_CODE));
			main->AddChild(login);
			
			AddChild(main);
		}
		~LoginDialog()
		{
				
		}
	private:
		void
		MessageReceived(BMessage* msg)
		{
			switch(msg->what)
			{
				case LOGIN_CODE:
				{
					BString code(code->Text());
					std::cout << "Google Code is " << code << std::endl;
					auth->NextStep(code);
					Quit();
					break;
				}
				/*case CANCEL:
				{
					Quit();
				}*/
				default:
					BWindow::MessageReceived(msg);
			}
		}
		BTextControl* code;
		BButton* login;
		TaskSync* auth;
};

#endif
