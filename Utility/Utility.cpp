#include "Utility.h"

#include <sstream>

#include <SupportDefs.h>


template<typename T>
BString
ToString(T arg)
{
	std::ostringstream str;
	str << arg;
	return str.str().c_str();
}


BString
GetNextId()
{
	static uint64 counter = 0;
	counter++;
	return "!@#" + ToString(counter);
}


BString
operator+(const BString& a, const BString& b)
{
	BString ab(a);
	ab += b;
	return ab;	
}


bool
CheckJsonKeys(json::Object& object,
	const std::vector<std::string>& keys) const
{
	for (auto& key : keys)
		if (object[key].GetType() == json::NULLVal)
			return false;
	return true;
}
