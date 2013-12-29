#ifndef OAUTH2_H
#define OAUTH2_H


#include <String.h>

/*!  ------ HOW TO USE
 * After creating an object assign fAuthorizeUrl, fTokenUrl, fClientId,
 * fClientSecret and fScope. Then call CallInternetBrowserForCode().
 * User should got key and give us - assign it to fAuthorizeCode. 
 * Now you can call GetAccessToken(). RefreshAccessToken() will be called
 * internally by GetAccessToken(), if it is needed.
 */

class OAuth2 {
	public:
									OAuth2();
	
		BString						GetAccessToken();
		bool						RefreshAccessToken();
		bool						CallInternetBrowserForCode() const;
	
	
	public:
		// Setter + getter = public variable
		BString						fAuthorizeUrl;
		BString						fTokenUrl;
		
		BString						fAuthroizeCode;
		BString						fClientId;
		BString						fClientSecret;
		BString						fScope;
		
		
	private:
		BString						fAccessToken;
		BString						fRefreshToken;
		
		time_t						fExpirationTime;
		
		const BString				fRedirectUri;
		const BString				fTokenType;
		
};

#endif // OAUTH2_H
