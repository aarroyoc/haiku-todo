#ifndef OAUTH2_H
#define OAUTH2_H


#include <String.h>

/*!  ------ HOW TO USE
 * After creating an object assign fAuthorizationUrl, fTokenUrl, fClientId,
 * fClientSecret and fScope. Then call CallInternetBrowserForCode().
 * User should got key and give us - assign it to fAuthorizeCode. 
 * Now you can call GetAccessToken().
 */

class OAuth2 {
	public:
									OAuth2();
	
		BString						GetUrlForAuthorization() const;
		bool						CallInternetBrowserForCode() const;
		BString						GetAccessToken();
	
	
	public:
		// Setter + getter = public variable
		BString						fAuthorizationUrl;
		BString						fTokenUrl;
		
		BString						fAuthorizationCode;
		BString						fClientId;
		BString						fClientSecret;
		BString						fScope;
		
		const BString				kTokenType;
		
	
	public:
		bool						_RefreshAccessToken();
		size_t						_CurlWriteFunction(void* ptr, size_t size,
										size_t nmemb, void* userData);
		
	private:
		BString						fAccessToken;
		BString						fRefreshToken;
		time_t						fExpirationTime;
		
		BString						fWriteFunctionBuffer;
		
		const BString				kRedirectUri;
		
		
		// Wrapper for receiving data is needed, because lambda expressions
		// causes crashes :( 
		static OAuth2*				sThis;
		static size_t				_WriteFunctionWrapper(void* ptr,
										size_t size, size_t nmemb,
										void* userData);
		
};

#endif // OAUTH2_H
