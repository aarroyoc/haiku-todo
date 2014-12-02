#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <Window.h>
#include <InterfaceKit.h>
#include <Application.h>
#include <SupportDefs.h>

#include "TaskListManager.h"

class MainWindow : public BWindow{
	public:
				MainWindow();
				~MainWindow();
	private:
		bool 	QuitRequested();
		void	MessageReceived(BMessage* msg);
		AppEngine::TaskListManager* manager;
		BView* tasks;
		BView* taskdetail;
		BListView* tasklist;
};

const uint32 ITEM_SELECTED=1;

#endif
