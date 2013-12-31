#include "Internet.h"

#include <curl/curl.h>

#include <iostream>


//////////////////////////////////////////////////////////////////////////////
//
//	Checking connection and getting time
//
//////////////////////////////////////////////////////////////////////////////


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
	
	curl_easy_cleanup(curl);
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


//////////////////////////////////////////////////////////////////////////////
//
//	Sending HTTP requests
//
//////////////////////////////////////////////////////////////////////////////


BString
Internet::SendHttpRequest(HttpMethod method, BString url,
	std::vector<BString> headers, BString data)
{	
	CURL* curl = curl_easy_init();
	if (curl == nullptr) {
		std::string message =
			"Internet::SendHttpRequest() :: cannot init curl";
		std::cerr << message << std::endl;
		throw std::runtime_error(message);
	}
	
	// Create response buffer and set curl write function
	BString responseBuffer;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _CurlWriteFunction);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBuffer);
	
	// Prepare HTTP headers
	curl_slist* curlHeaders = nullptr;
	for (auto it : headers)
		curlHeaders = curl_slist_append(curlHeaders, it.String());
	if (curlHeaders != nullptr)
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curlHeaders);
	
	switch (method) {
		case HttpMethod::GET:
			url += '?' + data;
			break;
			
		case HttpMethod::POST:
			curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.String());
			break;
			
		case HttpMethod::PUT:
			curl_easy_setopt(curl, CURLOPT_READFUNCTION, _CurlPutReadFunction);
			curl_easy_setopt(curl, CURLOPT_READDATA, &data);
			break;
			
		case HttpMethod::DELETE:
			// just do nothing -- DELETE does *NOT* send data 
			break;
	}
	
	// Here, because GET appends data to URL!
	curl_easy_setopt(curl, CURLOPT_URL, url.String());
	
	
	CURLcode res;
	res = curl_easy_perform(curl);
	
	// Always cleanup!
	curl_slist_free_all(curlHeaders);
	curl_easy_cleanup(curl);
	
	// Check whether request succeeded
	if (res != CURLE_OK) {
		std::string message = 
			std::string("Internet::SendHttpRequest() :: curl error: ")
			+ curl_easy_strerror(res); 
		std::cerr << message << std::endl;
		throw std::runtime_error(message);	
	}
	
	return responseBuffer;
}


size_t
Internet::_CurlWriteFunction(void* ptr, size_t size, size_t nmemb,
	void* userData)
{
	BString* requestResponse = reinterpret_cast<BString*>(userData);
	
	int32 oldSize = requestResponse->Length();
	int32 bufferSize = oldSize + size*nmemb;
	char* buffer = requestResponse->LockBuffer(bufferSize);
	
	memcpy(buffer + oldSize, ptr, size*nmemb);
	requestResponse->UnlockBuffer();
	return size*nmemb;
}

size_t
Internet::_CurlPutReadFunction(void* ptr, size_t size, size_t nmemb,
	void* userData)
{
	BString* requestData = reinterpret_cast<BString*>(userData);
	size_t curlSize = size*nmemb;
	size_t awaitingDataSize = requestData->Length();
	
	size_t toCopy = curlSize < awaitingDataSize ? curlSize : awaitingDataSize;
	char* buffer = requestData->LockBuffer(toCopy);
	memcpy(ptr, buffer, toCopy);
	requestData->UnlockBuffer();
	requestData->Remove(0, toCopy);
	
	return toCopy; 
}
