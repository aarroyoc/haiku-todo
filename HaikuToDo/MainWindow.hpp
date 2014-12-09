#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <Window.h>
#include <InterfaceKit.h>
#include <Application.h>
#include <SupportDefs.h>
#include <Layout.h>
#include <LayoutItem.h>

#include "TaskGoogle.hpp"

#ifdef BUILD_SQLITE
#include "TaskSQL.hpp"
#else
#include "TaskFS.hpp"
#endif

class MainWindow : public BWindow{
	public:
						MainWindow();
						~MainWindow();
	private:
		bool 			QuitRequested();
		void			MessageReceived(BMessage* msg);
		BView* 			tasks;
		BView* 			taskdetail;
		BListView*		categories;
		BListView* 		tasklist;
		BStringView*	taskTitle;
		BTextView*	 	taskDescription;
		BCheckBox* 		taskCompleted;
		BButton* 		taskEdit;
		BButton*		taskAdd;
		BButton*		taskRemove;
		BPicture*		taskImage;
		TaskManager*	manager;
		TaskSync*		sync;
		
};

const uint32 ITEM_SELECTED=1;
const uint32 COMPLETED_TASK=2;
const uint32 TASK_ADD=3;
const uint32 TASK_REMOVE=4;
const uint32 RELOAD=5;
const uint32 CHANGE_CATEGORY=6;
const uint32 RELOAD_CATEGORIES=7;

#endif
