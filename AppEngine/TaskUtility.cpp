#include "TaskUtility.h"

#include <sstream>

#include <SupportDefs.h>


namespace AppEngine {

template<typename T>
std::string
ToString(T arg)
{
	std::ostringstream str;
	str << arg;
	return str.str();
}


std::string
_GetNextId()
{
	static uint64 counter = 0;
	counter++;
	return "!@#" + ToString(counter);
}

} // namespace AppEngine
