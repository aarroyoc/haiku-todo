#ifndef TIMERFC3339_H
#define TIMERFC3339_H


#include <ctime>

#include <String.h>


/*! Class for converting unix time from and to RFC3339 timestamp.
 * General format: "YYYY-MM-DDTHH:MM:SS.MMMZ",
 * for example:	   "1970-01-01T00:00:00.000Z"
 */
class TimeRfc3339 {
	public:
								TimeRfc3339(time_t unixUtcTime = 0);
								TimeRfc3339(BString rfc3339Time);
		
		time_t					GetUnixUtcTime() const;
		BString					GetRfc3339Time() const;
		
		void					SetTime(time_t unixUtcTime);
		bool					SetTime(BString rfc3339Time);
		
		
	public:
		static bool				ValidateRfc3339String(BString rfc3339Time);
		
		
	private:
		static int32			_ToNumber(BString str, int32 from,
									int32 count);
		static BString			_ToString(int32 number, int32 charsCount);
		
		
	private:
		time_t					fTime;
			
};


#endif // TIMERFC3339_H
