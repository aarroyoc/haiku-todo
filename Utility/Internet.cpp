#include "Internet.h"

#include <curl/curl.h>

#include <iostream>


bool
Internet::CheckConnection()
{
	CURL *curl = curl_easy_init();
	
	if (curl) {
		CURLcode res;
	
		curl_easy_setopt(curl, CURLOPT_URL, "google.com");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION,
			[](char* ptr, size_t size, size_t nmemb, void* userData) 
			{ return  size * nmemb; ptr=ptr; userData=userData;} );
			// should mute printing to stdout... why does not it work?
			// PS instructions after return statement mutes warnings about
			//    unused variables
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			switch (res) {
				case CURLE_COULDNT_CONNECT:
				case CURLE_COULDNT_RESOLVE_HOST:
				case CURLE_COULDNT_RESOLVE_PROXY:
					return false;
					break;
				
				default:
					std::cerr << "Request failed:" << curl_easy_strerror(res)
						<< std::endl;
		            return false;
		    	}
		  }
	}
		  
	curl_easy_cleanup(curl);
	return true;
}


time_t
Internet::GetUtcTime()
{
	long time = -4;
	CURL* curl = curl_easy_init();
	
	if (curl) {
		CURLcode res;
		
		curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		curl_easy_setopt(curl, CURLOPT_FILETIME, 1);
		curl_easy_setopt(curl, CURLOPT_URL, "google.com");
		res = curl_easy_perform(curl);
		if (res != CURLE_OK)
			return -2;
		res = curl_easy_getinfo(curl, CURLINFO_FILETIME, &time);
		if (res != CURLE_OK)
			return -3;
	}
	
	return time;
}


time_t
Internet::GetCachedUtcTime()
{
	static long sDeltaTime = 0;
	static bool sDeltaCounted = false;
	
	time_t localTime = time(nullptr);
	time_t utcSystemTime = mktime(gmtime(&localTime));
	
	if (sDeltaCounted == false) {
		time_t internetTime = Internet::GetUtcTime();	
		if (internetTime >= 0) {
			sDeltaTime = internetTime - utcSystemTime;
			sDeltaCounted = true;	
		}
	}
	
	return utcSystemTime + sDeltaTime;
}
