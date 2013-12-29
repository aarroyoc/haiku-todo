#ifndef INTERNET_H
#define INTERNET_H


#include <ctime>


class Internet {
	public:
		static bool					CheckConnection();
		static time_t				GetUtcTime();
		static time_t				GetCachedUtcTime();
			// Gets right time if GetUtcTime() will internal success once and
			// then user won't change system time.
};

#endif // INTERNET_H
