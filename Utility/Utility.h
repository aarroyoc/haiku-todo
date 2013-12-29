#include <String.h>

#include "json.h"


template<typename T>
BString ToString(T arg);

BString GetNextId();

BString operator+(const BString& a, const BString& b);

bool CheckJsonKeys(json::Object& object, const std::vector<std::string>& keys);
