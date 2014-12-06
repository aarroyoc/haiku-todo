#ifndef ADDTASK_HPP
#define ADDTASK_HPP

const int32 SAVE_TASK=10;
const int32 CANCEL=200;
const int32 CREATE_CATEGORY=300;
const int32 SAVE_CATEGORY=400;
const int32 SELECT_ICON=500;

#include <InterfaceKit.h>
#include <Layout.h>
#include <LayoutItem.h>
#include <LayoutBuilder.h>
#ifdef BUILD_SQLITE
#include "TaskSQL.hpp"
#else
#include "TaskFS.hpp"
#endif
#include "MainWindow.hpp"
#include "CreateCategory.hpp"
#include "Category.hpp"



class AddTask : public BWindow{
	public:
		AddTask(TaskManager* manager) : BWindow(BRect(100,100,450,500),
			"Add a task",B_MODAL_WINDOW,B_NOT_MINIMIZABLE| B_NOT_RESIZABLE),
			manager(manager)
		{
			BView* main=new BView(Bounds(),"Main view",B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
			main->SetViewColor(220,220,220);
			
			BGridLayout* layout=new BGridLayout();
			main->SetLayout(layout);
			
			title=new BTextControl("Title","Title: ","",NULL);
			layout->AddView(title,0,0,8,1);
			
			description=new BTextView("Description");
			BScrollView* scroll=new BScrollView("Description scroll",description,0,false,true);
			layout->AddView(scroll,0,1,8,3);
			
			categories=new BListView("Categories",B_SINGLE_SELECTION_LIST);
			manager->LoadCategories(categories);
			BScrollView* scrollCategories=new BScrollView(NULL,categories,0,false,true);
			layout->AddView(scrollCategories,0,4,8,2);
			
			save=new BButton(NULL,"Save task",new BMessage(SAVE_TASK));
			layout->AddView(save,0,6,4,1);
			
			cancel=new BButton(NULL,"Cancel",new BMessage(CANCEL));
			layout->AddView(cancel,4,6,2,1);
			
			createCategory=new BButton(NULL,"Create category",new BMessage(CREATE_CATEGORY));
			layout->AddView(createCategory,6,6,2,1);
			
			layout->SetInsets(10.0f,10.0f,10.0f,10.0f);
			
			
			AddChild(main);
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
					BString category("ALL");
					int32 selection=categories->CurrentSelection();
					if(selection>=0)
					{
						Category* cat=dynamic_cast<Category*>(categories->ItemAt(selection));
						category=cat->GetName();
					}
					bool rc=manager->AddTask(title->Text(),description->Text(),category);
					if(!rc)
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
				case CREATE_CATEGORY:
				{
					CreateCategory* create=new CreateCategory(manager);
					create->Show();
					PostMessage(new BMessage(RELOAD));
				}
				case RELOAD_CATEGORIES:
				{
					categories->MakeEmpty();
					manager->LoadCategories(categories);
				}
				default:
					BWindow::MessageReceived(msg);
			}
		}
		BTextControl*	title;
		BTextView*		description;
		BButton*		save;
		BButton*		cancel;
		BListView*		categories;
		BButton*		createCategory;
		TaskManager*	manager;
};

#endif
