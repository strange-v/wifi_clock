#include "TimeSync.h"

bool syncTime()
{
	bool result = timeClient.update();
	if (result)
	{
		unsigned long epoch = timeClient.getEpochTime();
		time_t local = timezone->toLocal(epoch - 946684800l); //ToDo: Move this constatn to an appropriate place

		RtcDateTime date = RtcDateTime(local);
		Rtc.SetDateTime(date);
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
	return result;
}

void _syncTime(uint32_t deltaTime)
{
	if (syncTime())
	{
		taskManager.StopTask(&taskSyncTime);
	}
}
