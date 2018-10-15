#include "TimeSync.h"

void syncTime()
{
	if (timeClient.update())
	{
		unsigned long epoch = timeClient.getEpochTime();
		//timeClient.setTimeOffset(3);

		Rtc.SetDateTime(RtcDateTime(epoch));
#ifdef _DEBUG
		Serial.print(F("Time: "));
		Serial.println(timeClient.getFormattedTime());
#endif
	}
	else
	{
#ifdef _DEBUG
		Serial.println(F("NTP error"));
#endif
	}
	
}
