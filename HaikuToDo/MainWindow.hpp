#ifndef MAIN_WINDOW_HPP
#define MAIN_WINDOW_HPP

#include <Window.h>
#include <InterfaceKit.h>
#include <Application.h>
#include <SupportDefs.h>

class MainWindow : public BWindow{
	public:
				MainWindow();
				~MainWindow();
	private:
		bool 	QuitRequested();
		void	MessageReceived(BMessage* msg);
};

#endif
