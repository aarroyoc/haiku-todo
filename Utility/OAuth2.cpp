#include "OAuth2.h"
#include "Utility.h"

#include <cstdlib>
#include <iostream>
#include <stdexcept>

#include <curl/curl.h>

#include "json.h"


//////////////////////////////////////////////////////////////////////////////
//
//	Static members (due to lambda expressions crashes...)
//
//////////////////////////////////////////////////////////////////////////////


OAuth2* OAuth2::sThis = nullptr;

size_t OAuth2::_WriteFunctionWrapper(void* ptr, size_t size, size_t nmemb,
	void* userData)
{
	if (sThis != nullptr)
		return sThis->_CurlWriteFunction(ptr, size, nmemb, userData);
	return size * nmemb;
}


//////////////////////////////////////////////////////////////////////////////
//
//	Constructor
//
//////////////////////////////////////////////////////////////////////////////


OAuth2::OAuth2()
	:
	kTokenType("Bearer"),
	fExpirationTime(0),
	kRedirectUri("urn:ietf:wg:oauth:2.0:oob")
{
}


//////////////////////////////////////////////////////////////////////////////
//
//	Public methods
//
//////////////////////////////////////////////////////////////////////////////


BString
OAuth2::GetUrlForAuthorization() const
{
	BString responseType = "code";
	if (fAuthorizationUrl == "" || fClientId == "" || fScope == "") {
		std::cerr << "OAuth2 :: Missing authorize url, client id and/or scope"
			<< std::endl;
		return "";
	}
	
	BString url = fAuthorizationUrl
		+ "?response_type=" + responseType
		+ "&client_id=" + fClientId
		+ "&redirect_uri=" + kRedirectUri
		+ "&scope=" + fScope;
	
	return url;
}


bool
OAuth2::CallInternetBrowserForCode() const
{
	BString url = GetUrlForAuthorization();
	if (url == "")
		return false;
	
	int errorCode = system("WebPositive \"" + url + "\" >/dev/null 2>&1");
		// find better way... BRoaster?
	
	return errorCode == 0;
}


BString
OAuth2::GetAccessToken()
{
	if (fAccessToken != "") {
		time_t currentTime = time(nullptr);
		time_t refreshSecondsBefore = 300;
			// Get new token 5 minutes before old expiration
		
		if (currentTime < fExpirationTime - refreshSecondsBefore)
			return fAccessToken;
		if (currentTime < fExpirationTime) {
			_RefreshAccessToken();
				// if fail, old token is still valid
			return fAccessToken;	
		}
		if (_RefreshAccessToken() == true)
			return fAccessToken;
	}
	
	if (fAuthorizationCode == "" || fClientId == "" || fClientSecret == "" ||
		fTokenUrl == "") {
		std::cerr << "OAuth2 :: Missing authorizaton code, client id, client "
			"secret and/or url for requesting" << std::endl;
		return "";
	}
	
	BString grantType = "authorization_code";
	BString postData = "code=" + fAuthorizationCode
		+ "&client_id=" + fClientId
		+ "&client_secret=" + fClientSecret
		+ "&redirect_uri=" + kRedirectUri
		+ "&grant_type=" + grantType;
		
	
	CURL* curl = curl_easy_init();
	CURLcode res;
	if (curl == nullptr) {
		std::cerr << "OAuth2 :: Cannot init curl" << std::endl;
		return "";
	}
	
	sThis = this;
	curl_easy_setopt(curl, CURLOPT_URL, fTokenUrl.String());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.String());
	//curl_easy_setopt(curl, CURLOPT_USE_SLL, CURLUSESLL_CONTROL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _WriteFunctionWrapper);
		//	[&](void* ptr, size_t size, size_t nmemb, void* userData) {
		//		return this->_CurlWriteFunction(ptr, size, nmemb, userData);
		//	});
			// lambda proxy crashes app :(
	fWriteFunctionBuffer = ""; // clean buffer

	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		std::cerr << "OAuth2 :: Curl perform error: " << std::endl
			<< curl_easy_strerror(res) << std::endl;
		curl_easy_cleanup(curl);
		return "";
	}
	
	json::Object response;
	try {
		std::string buffer = fWriteFunctionBuffer.String();
		response = json::Deserialize(buffer);
	}
	catch(std::runtime_error& e) {
		std::cerr << "OAuth2 :: Invalid JSON server response: " << std::endl
			<< fWriteFunctionBuffer.String() << std::endl;
		return "";	
	}
	
	std::vector<std::string> keys = {"access_token", "token_type",
		"expires_in", "refresh_token"};
	if (CheckJsonKeys(response, keys) == false) {
		std::cerr << "OAuth2 :: Invalid JSON server response: "
			<< fWriteFunctionBuffer.String() << std::endl;
		return "";
	}
	
	if (static_cast<std::string>(response["token_type"])
		!= kTokenType.String()) {
		std::cerr << "OAuth2 :: Invalid token type: "
			<< static_cast<std::string>(response["token_type"]) << std::endl;
		return "";
	}
	
	fAccessToken = static_cast<std::string>(response["access_token"]).c_str();
	fRefreshToken = static_cast<std::string>(response["refresh_token"]).c_str();
	fExpirationTime = time(nullptr) + static_cast<int>(response["expires_in"]);
	
	curl_easy_cleanup(curl);
	return fAccessToken;
}


//////////////////////////////////////////////////////////////////////////////
//
//	Private methods
//
//////////////////////////////////////////////////////////////////////////////


bool
OAuth2::_RefreshAccessToken()
{	
	if (fRefreshToken == "" || fClientId == "" || fClientSecret == "" ||
		fTokenUrl == "") {
		std::cerr << "OAuth2 :: Missing refresh token, client id, client "
			"secret and/or url for requesting" << std::endl;
		return false;
	}
	
	BString grantType = "refresh_token";
	BString postData = "client_id=" + fClientId
		+ "&client_secret=" + fClientSecret
		+ "&refresh_token=" + fRefreshToken
		+ "&grant_type=" + grantType;
		
	
	CURL* curl = curl_easy_init();
	CURLcode res;
	if (curl == nullptr) {
		std::cerr << "OAuth2 :: Cannot init curl" << std::endl;
		return false;
	}
	
	sThis = this;
	curl_easy_setopt(curl, CURLOPT_URL, fTokenUrl.String());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postData.String());
	//curl_easy_setopt(curl, CURLOPT_USE_SLL, CURLUSESLL_CONTROL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _WriteFunctionWrapper);
		//	[&](void* ptr, size_t size, size_t nmemb, void* userData) {
		//		return this->_CurlWriteFunction(ptr, size, nmemb, userData);
		//	});
			// lambda proxy crashes app :(
	fWriteFunctionBuffer = ""; // clean buffer

	res = curl_easy_perform(curl);
	if (res != CURLE_OK) {
		std::cerr << "OAuth2 :: Curl perform error: " << std::endl
			<< curl_easy_strerror(res) << std::endl;
		curl_easy_cleanup(curl);
		return false;
	}
	
	json::Object response;
	try {
		std::string buffer = fWriteFunctionBuffer.String();
		response = json::Deserialize(buffer);
	}
	catch(std::runtime_error& e) {
		std::cerr << "OAuth2 :: Invalid JSON server response: " << std::endl
			<< fWriteFunctionBuffer.String() << std::endl;
		return false;	
	}
	
	std::vector<std::string> keys = {"access_token", "token_type",
		"expires_in"};
	if (CheckJsonKeys(response, keys) == false) {
		std::cerr << "OAuth2 :: Invalid JSON server response: "
			<< fWriteFunctionBuffer.String() << std::endl;
		return false;
	}
	
	if (static_cast<std::string>(response["token_type"])
		!= kTokenType.String()) {
		std::cerr << "OAuth2 :: Invalid token type: "
			<< static_cast<std::string>(response["token_type"]) << std::endl;
		return false;
	}
	
	fAccessToken = static_cast<std::string>(response["access_token"]).c_str();
	fExpirationTime = time(nullptr) + static_cast<int>(response["expires_in"]);
	
	curl_easy_cleanup(curl);
	return true;
}


size_t
OAuth2::_CurlWriteFunction(void* ptr, size_t size, size_t nmemb,
	void* userData)
{
	int32 oldSize = fWriteFunctionBuffer.Length();
	int32 bufferSize = oldSize + size*nmemb;
	char* buffer = fWriteFunctionBuffer.LockBuffer(bufferSize);
	
	memcpy(buffer + oldSize, ptr, size*nmemb);
	fWriteFunctionBuffer.UnlockBuffer();
	return size*nmemb;
	
	userData = userData;
		// mute "unused viariable" warning
}
