#include "Internet.h"

#include <curl/curl.h>

#include <iostream>


bool
Internet::CheckConnection()
{
	CURL *curl = curl_easy_init();
	
	if(curl) {
		CURLcode res;
	
		curl_easy_setopt(curl, CURLOPT_URL, "www.google.com");
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
