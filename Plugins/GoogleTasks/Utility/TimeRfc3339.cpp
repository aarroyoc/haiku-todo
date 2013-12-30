#include "TimeRfc3339.h"
#include "Utility.h"


//////////////////////////////////////////////////////////////////////////////
//
//	Constructors
//
//////////////////////////////////////////////////////////////////////////////


bool
TimeRfc3339::ValidateRfc3339String(BString rfc3339Time)
{
	// Regex on Haiku doesn't work - throws exception with "regex_error"
	// string as what()
	
	int32 strLength = 24;
	if (rfc3339Time.Length() != strLength)
		return false;
	
	int32 numbersPos[] = {0,1,2,3,5,6,8,9,11,12,14,15,17,18,20,21,22};
	for (auto i : numbersPos)
		if (rfc3339Time[i] < '0' || rfc3339Time[i] > '9')
			return false;
	
	if (rfc3339Time[4] != '-' || rfc3339Time[7] != '-' ||
		rfc3339Time[10] != 'T' || rfc3339Time[13] != ':' ||
		rfc3339Time[16] != ':' || rfc3339Time[19] != '.' ||
		rfc3339Time[23] != 'Z')
		return false;
	
	return true;		
}


//////////////////////////////////////////////////////////////////////////////
//
//	Constructors
//
//////////////////////////////////////////////////////////////////////////////


TimeRfc3339::TimeRfc3339(time_t unixUtcTime)
	:
	fTime(unixUtcTime)
{
}


TimeRfc3339::TimeRfc3339(BString rfc3339Time)
{
	SetTime(rfc3339Time);
}


//////////////////////////////////////////////////////////////////////////////
//
//	Public methods - getters
//
//////////////////////////////////////////////////////////////////////////////


time_t
TimeRfc3339::GetUnixUtcTime() const
{
	return fTime;
}


BString
TimeRfc3339::GetRfc3339Time() const
{
	tm* utcTime = localtime(&fTime);
	int32 miliseconds = 0;
	BString rfc3339Time = _ToString(utcTime->tm_year + 1900, 4) + '-'
		+ _ToString(utcTime->tm_mon + 1, 2) + '-'
		+ _ToString(utcTime->tm_mday, 2) + 'T'
		+ _ToString(utcTime->tm_hour, 2) + ':'
		+ _ToString(utcTime->tm_min, 2) + ':'
		+ _ToString(utcTime->tm_sec, 2) + '.'
		+ _ToString(miliseconds, 3) + 'Z';
	
	return rfc3339Time;
}


//////////////////////////////////////////////////////////////////////////////
//
//	Public methods - setters
//
//////////////////////////////////////////////////////////////////////////////


void
TimeRfc3339::SetTime(time_t unixUtcTime)
{
	fTime = unixUtcTime;
}


bool
TimeRfc3339::SetTime(BString rfc3339Time)
{
	if (ValidateRfc3339String(rfc3339Time) == false)
		return false;
	
	tm time;
	time.tm_year =	_ToNumber(rfc3339Time, 0, 4) - 1900; // years since 1900
	time.tm_mon =	_ToNumber(rfc3339Time, 5, 2) - 1; // 0-11 (months sice Januar)
	time.tm_mday =	_ToNumber(rfc3339Time, 8, 2); // 1-31
	time.tm_hour =	_ToNumber(rfc3339Time, 11, 2) - 1; // 0-23
	time.tm_min =	_ToNumber(rfc3339Time, 14, 2) - 1; // 0-59
	time.tm_sec =	_ToNumber(rfc3339Time, 17, 2) - 1; // 0-60* (*leap seconds)
	
	time_t localTime = mktime(&time);
	time_t utcTime = mktime(gmtime(&localTime));
	
	fTime = utcTime;
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//
//	Private methods - converters
//
//////////////////////////////////////////////////////////////////////////////



int32
TimeRfc3339::_ToNumber(BString str, int32 from, int32 count)
{
	int result = 0;
	for (int i=0; i<count; i++)
		result = result*10 + str[from+i];
	return result;
}
									
									
BString
TimeRfc3339::_ToString(int32 number, int32 charsCount)
{
	BString str = ToString(number);	
	str = BString().SetTo('0', charsCount - str.Length()) + str;
	return str;
}
