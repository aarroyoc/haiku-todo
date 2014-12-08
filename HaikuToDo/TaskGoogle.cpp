#include "TaskSync.hpp"
#include "TaskGoogle.hpp"
#include <NetworkKit.h>
#include <UrlRequest.h>
#include <UrlSynchronousRequest.h>
#include <UrlProtocolRoster.h>
#include <HttpHeaders.h>
#include <HttpRequest.h>
#include <iostream>

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

BString DoHTTPRequest(BString http)
{
	BUrl url(http);
	SynchronousListener listener;
	BUrlRequest* request=BUrlProtocolRoster::MakeRequest(url,&listener);
	request->Run();
	while(request->IsRunning()) snooze(100);
	
	size_t bufsize = listener.result.BufferLength();
	char* buf = (char*)malloc(bufsize);
	memcpy(buf, listener.result.Buffer(), bufsize);
	
	delete request;
	
	BString response(buf);
	
	std::cout << "Response " << response.String() << std::endl;
	
	return response;
}

TaskGoogle::TaskGoogle()
{
	
}

TaskGoogle::~TaskGoogle()
{
	
}

bool
TaskGoogle::Login()
{
	BString endpoint("WebPositive 'https://accounts.google.com/o/oauth2/auth");
	endpoint.Append("?response_type=code");
	endpoint.Append("&client_id=318709342848-0h9712v3kbpcv1r7oc8krdrfu22ohlld.apps.googleusercontent.com");
	endpoint.Append("&redirect_uri=urn:ietf:wg:oauth:2.0:oob");
	endpoint.Append("&scope=https://www.googleapis.com/auth/tasks' &");
	std::cout << endpoint.String() << std::endl;
	system(endpoint.String());
	
	LoginDialog* login=new LoginDialog(this);
	login->Show();
	
}

void
TaskGoogle::NextStep(BString code)
{
	BString tokenUrl("code=");
	tokenUrl.Append(code);
	tokenUrl.Append("&client_id=");
	tokenUrl.Append("&client_secret=");
	tokenUrl.Append("&redirect_uri=");
	tokenUrl.Append("&grant_type=authorization_code");
	
	BUrl url("https://www.googleapis.com/oauth2/v3/token");
	
	SynchronousListener listener;
	BHttpRequest req(url,true,"HTTP",&listener);
	
	//BHttpHeaders headers;
	//req.SetHeaders(headers);
	
	BHttpForm form;
	form.AddString("code",BUrl::UrlEncode(code));
	form.AddString("client_id",BUrl::UrlEncode(BString("318709342848-0h9712v3kbpcv1r7oc8krdrfu22ohlld.apps.googleusercontent.com")));
	form.AddString("client_secret",BUrl::UrlEncode(BString("WyyNzE2JO-HUQqL5RG2VYzz2")));
	form.AddString("grant_type",BUrl::UrlEncode(BString("authorization_code")));
	form.AddString("redirect_uri",BUrl::UrlEncode(BString("urn:ietf:wg:oauth:2.0:oob")));
	req.SetPostFields(form);
	
	req.SetMethod(B_HTTP_POST);
	
	
	req.Run();
	while(req.IsRunning()) snooze(100);
	
	size_t bufsize = listener.result.BufferLength();
	char* buf = (char*)malloc(bufsize);
	memcpy(buf, listener.result.Buffer(), bufsize);
	
	//delete req;
	
	BString response(buf);
	
	std::cout << "Response " << response.String() << std::endl;
	
}
