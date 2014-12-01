#include "MainWindow.hpp"

MainWindow::MainWindow() 
	: BWindow(BRect(50,50,450,450),"HaikuToDo",B_TITLED_WINDOW,0)
{
	
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
		default:
			BWindow::MessageReceived(msg);
	}
}
