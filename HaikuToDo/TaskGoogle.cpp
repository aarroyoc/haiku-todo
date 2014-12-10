#include "TaskSync.hpp"
#include "TaskGoogle.hpp"
#include <NetworkKit.h>
#include <UrlRequest.h>
#include <UrlSynchronousRequest.h>
#include <UrlProtocolRoster.h>
#include <HttpHeaders.h>
#include <HttpRequest.h>
#include <private/shared/Json.h>
#include <iostream>
#include "Internet.hpp"
#include <string>
#include <sstream>
#include <app/Roster.h>

TaskGoogle::TaskGoogle()
{
	categories=new BList(20);
	tasks=new BList(20);
}

TaskGoogle::~TaskGoogle()
{
	
}

bool
TaskGoogle::Login()
{
	BString endpoint("https://accounts.google.com/o/oauth2/auth");
	endpoint.Append("?response_type=code");
	endpoint.Append("&client_id=318709342848-0h9712v3kbpcv1r7oc8krdrfu22ohlld.apps.googleusercontent.com");
	endpoint.Append("&redirect_uri=urn:ietf:wg:oauth:2.0:oob");
	endpoint.Append("&scope=https://www.googleapis.com/auth/tasks");
	std::cout << endpoint.String() << std::endl;
	const char *args[] = { endpoint.String(), 0 };
	be_roster->Launch("application/x-vnd.Be.URL.http",1,const_cast<char **>(args));
	
	LoginDialog* login=new LoginDialog(this);
	login->Show();
	
}

void
TaskGoogle::NextStep(BString code)
{
	BHttpForm* form=new BHttpForm();
	form->AddString("code",code);
	form->AddString("client_id","318709342848-0h9712v3kbpcv1r7oc8krdrfu22ohlld.apps.googleusercontent.com");
	form->AddString("client_secret","WyyNzE2JO-HUQqL5RG2VYzz2");
	form->AddString("grant_type","authorization_code");
	form->AddString("redirect_uri","urn:ietf:wg:oauth:2.0:oob");
	form->SetFormType(B_HTTP_FORM_URL_ENCODED);
	
	BString oauth2("https://www.googleapis.com/oauth2/v3/token");
	BString tokenResponse(HaikuHTTP::GET(oauth2,form));
	
	BMessage tokenJson;
	BPrivate::BJson::Parse(tokenJson,tokenResponse);
	tokenJson.PrintToStream();
	
	token=BString(tokenJson.GetString("access_token","NOT_FOUND"));
	std::cout << "Token access " << token.String() << std::endl;

	
	BString listUrlString("https://www.googleapis.com/tasks/v1/users/@me/lists?access_token=");
	listUrlString.Append(token);
	
	BString listsResponse(HaikuHTTP::GET(listUrlString));
	
	BMessage listsJson;
	std::cout << listsResponse << std::endl;
	BPrivate::BJson::Parse(listsJson,listsResponse);
	listsJson.PrintToStream();
	
	BMessage userLists;
	if(listsJson.FindMessage("items",0,&userLists)!=B_OK)
	{
		std::cerr << "ERROR: 'items' not found" << std::endl;
	}

	
	int32 lists=userLists.CountNames(B_ANY_TYPE);
	std::cout << "Lists found: " << lists << std::endl;
	
	for(int32 currentList=0;currentList<lists;currentList++)
	{
		std::ostringstream ss;
		ss << currentList;
		BMessage userList;
		if(userLists.FindMessage(ss.str().c_str(),&userList)!=B_OK)
		{
			std::cerr << "ERROR: No '0' list found " << std::endl;
		}
		userList.PrintToStream();
		const char* title;
		const char* id;
		if(userList.FindString("title",&title)!=B_OK)
		{
			std::cerr << "ERROR: No 'title' found " << std::endl;
		}
		userList.FindString("id",&id);
		Category* cat=new Category(title,"MIME_DATABASE",id);
		categories->AddItem(cat);
		this->GetTasks(cat);
	}
	
	//DISPATCH SYNC_CATEGORIES
	
	int32 count=be_app->CountWindows();
	for(int32 i=0;i<count;i++)
	{
		be_app->WindowAt(i)->PostMessage(new BMessage(SYNC_CATEGORIES));
	}

}

BList*
TaskGoogle::GetCategories()
{
	return categories;
}

BList*
TaskGoogle::GetTasks(Category* cat)
{
	//DO HTTP CONNECTION TO GOOGLE
	BList* tks=new BList(20);
	
	BString id=cat->GetID();
	std::cout << "ID: " << id.String() << std::endl;
	BString url("https://www.googleapis.com/tasks/v1/lists/");
	url.Append(id);
	url.Append("/tasks?access_token=");
	url.Append(token);
	
	BString response(HaikuHTTP::GET(url));
	std::cout << response.String() << std::endl;
	BMessage taskJson;
	BPrivate::BJson::Parse(taskJson,response);
	taskJson.PrintToStream();
	
	BMessage items;
	taskJson.FindMessage("items",&items);
	int32 lists=items.CountNames(B_ANY_TYPE);
	std::cout << "Lists found: " << lists << std::endl;
	
	for(int32 currentTask=0;currentTask<lists;currentTask++)
	{
		std::ostringstream ss;
		ss << currentTask;
		BMessage task;
		if(items.FindMessage(ss.str().c_str(),&task)!=B_OK)
		{
			std::cerr << "ERROR: No '0' list found " << std::endl;
		}
		task.PrintToStream();
		const char* title;
		const char* taskId;
		const char* notes;
		const char* status;
		task.FindString("title",&title);
		task.FindString("id",&taskId);
		task.FindString("notes",&notes);
		task.FindString("status",&status);
		Task* tk=new Task(title,notes,id.String(),false);
		tks->AddItem(tk);
	}
	
	
	return tks;
}
