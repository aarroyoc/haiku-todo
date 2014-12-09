#ifndef INTERNET_HPP
#define INTERNET_HPP

#include <NetworkKit.h>
#include <UrlRequest.h>
#include <UrlSynchronousRequest.h>
#include <UrlProtocolRoster.h>
#include <HttpHeaders.h>
#include <HttpRequest.h>
#include <private/shared/Json.h>

class SynchronousListener: public BUrlProtocolListener
{
 public:
 virtual ~SynchronousListener() {};
 void DataReceived(BUrlRequest*, const char* data, off_t position,
 ssize_t size) {
 result.WriteAt(position, data, size);
 }
 BMallocIO result;
};

class HaikuHTTP {
	public:
		static BString GET(BString urlString,BHttpForm* form = NULL)
		{
			BUrl url(urlString);
			SynchronousListener listener;
			BHttpRequest req(url,true,"HTTP",&listener);
			if(form != NULL)
			{
				req.AdoptPostFields(form);
			}
			req.Run();
			while(req.IsRunning()) snooze(100);
			
			size_t bufsize = listener.result.BufferLength();
			char* buf = (char*)malloc(bufsize);
			memcpy(buf, listener.result.Buffer(), bufsize);	
			BString response(buf);
			
			return response;
		}
};

#endif
