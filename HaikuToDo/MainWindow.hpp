#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <Window.h>
#include <InterfaceKit.h>
#include <Application.h>
#include <SupportDefs.h>

#include "TaskLocal.hpp"

class MainWindow : public BWindow{
	public:
						MainWindow();
						~MainWindow();
	private:
		bool 			QuitRequested();
		void			MessageReceived(BMessage* msg);
		BView* 			tasks;
		BView* 			taskdetail;
		BListView* 		tasklist;
		BStringView*	taskTitle;
		BStringView* 	taskDescription;
		BCheckBox* 		taskCompleted;
		BButton* 		taskEdit;
		BButton*		taskAdd;
		BButton*		taskRemove;
		BPicture*		taskImage;
		TaskLocal*		manager;
		
};

const uint32 ITEM_SELECTED=1;
const uint32 COMPLETED_TASK=2;
const uint32 TASK_ADD=3;
const uint32 TASK_REMOVE=4;
const uint32 RELOAD=5;

#endif
