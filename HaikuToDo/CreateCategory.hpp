#ifndef CREATE_CATEGORY_HPP
#define CREATE_CATEGORY_HPP

#include "AddTask.hpp"

const int32 ICON_SELECTED=600;

class CreateCategory : public BWindow{
	public:
		CreateCategory(TaskLocal* manager) : BWindow(BRect(100,100,300,300),"Create category"
			,B_MODAL_WINDOW,0), manager(manager)
		{
			BView* main=new BView(Bounds(),"Main",B_FOLLOW_ALL_SIDES,B_WILL_DRAW);
			BGroupLayout* layout=new BGroupLayout(B_VERTICAL);
			main->SetViewColor(220,220,220);
			main->SetLayout(layout);
			
			name=new BTextControl(NULL,"Name:","",NULL);
			layout->AddView(name);
			
			selectfile=new BButton(NULL,"Select icon",new BMessage(SELECT_ICON));
			layout->AddView(selectfile);
			
			save=new BButton(NULL,"Save",new BMessage(SAVE_CATEGORY));
			layout->AddView(save);
			
			cancel=new BButton(NULL,"Cancel",new BMessage(CANCEL));
			layout->AddView(cancel);
			
			AddChild(main);
		}
		~CreateCategory()
		{
			
		}
	private:
		void
		MessageReceived(BMessage* msg)
		{
			switch(msg->what)
			{
				case SELECT_ICON:
				{
					BFilePanel* panel=new BFilePanel(B_OPEN_PANEL,
						new BMessenger(this),NULL,B_FILE_NODE,false,
						new BMessage(ICON_SELECTED),NULL,true);
					panel->Show();
					break;
				}
				case ICON_SELECTED:
				{
					if(msg->HasRef("refs"))
					{
						entry_ref ref;
						if(msg->FindRef("refs",0,&ref) == B_OK)
						{
							BEntry entry(&ref,true);
							BPath path;
							entry.GetPath(&path);
							filename=BString(path.Path());
							selectfile->SetLabel(filename);
						}
					}
					break;
				}
				case SAVE_CATEGORY:
				{
					manager->AddCategory(name->Text(),filename.String());
					int32 count=be_app->CountWindows();
					for(int32 i=0;i<count;i++)
					{
						be_app->WindowAt(i)->PostMessage(new BMessage(RELOAD_CATEGORIES));
					}
				}
				case CANCEL:
				{
					Quit();
				}
			}
		}
		
		BTextControl* 	name;
		BButton*		selectfile;
		BButton*		save;
		BButton*		cancel;
		BString 		filename;
		TaskLocal*		manager;
};

#endif
