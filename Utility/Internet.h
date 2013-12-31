#ifndef INTERNET_H
#define INTERNET_H


#include <ctime>
#include <vector>

#include <String.h>


class Internet {
	public:
		static bool					CheckConnection();
		static time_t				GetUtcTime();
		static time_t				GetCachedUtcTime();
			// Gets right time if GetUtcTime() will internal success once and
			// then user won't change system time.
		
		
	public:
		enum class HttpMethod {
			GET,
			POST,
			PUT,
			DELETE
		};
		
		static BString				SendHttpRequest(
										HttpMethod	method,
										BString		url,
										std::vector<BString>
													headers =
														std::vector<BString>(),
										BString		data = ""
									);
	
	private:
		static size_t				_CurlWriteFunction(void* ptr, size_t size,
										size_t nmemb, void* userData);
		
		static size_t				_CurlPutReadFunction(void* ptr,
										size_t size, size_t nmemb,
										void* userData);
};

#endif // INTERNET_H
