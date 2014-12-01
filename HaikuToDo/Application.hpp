#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <Application.h>

#include "MainWindow.hpp"

class HaikuToDo : public BApplication{
	public:
				HaikuToDo();
				~HaikuToDo();
		void	ReadyToRun();	
};

#endif
