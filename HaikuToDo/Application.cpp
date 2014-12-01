/**
 * HaikuToDo
 *
 */

#include "Application.hpp"

HaikuToDo::HaikuToDo() : BApplication("application/haiku-todo")
{
	
}

HaikuToDo::~HaikuToDo()
{
	
}

void
HaikuToDo::ReadyToRun()
{
	MainWindow* window=new MainWindow();
	window->Show();
}
