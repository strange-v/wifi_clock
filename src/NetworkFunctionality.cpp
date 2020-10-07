#include "NetworkFunctionality.h"

void connectToNetwork(bool fallbackToAp)
{
	ehNetworkConnected = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP &event) {
		if (_connectionState == CS_CONNECTING_W_AP)
		{
			initWebServer();
		}
		_connectionState = CS_CONNECTED;
#ifdef CL_DEBUG
		Serial.println();
		printNetworkInfo();
#endif
		taskManager.StartTask(&taskDelayedSyncTime);
	});

	_connectToNetwork(fallbackToAp);
}

void startSoftAP()
{
	char ssid[6];
	memcpy(ssid, getSsid(), sizeof(ssid));
#ifdef CL_DEBUG
	Serial.println();
	Serial.print(F("Start AP: "));
	Serial.println(ssid);
#endif

	WiFi.softAP(ssid, Cfg::wifiApPassword, 6);
	taskManager.StartTask(&taskTurnOfNetwork);
	_connectionState = CS_ACCESS_POINT;
	_clockMode = CM_CONFIG;

	initWebServer();

#ifdef CL_DEBUG
	IPAddress myIP = WiFi.softAPIP();
	Serial.print(F("AP IP address: "));
	Serial.println(myIP);
#endif
}

bool syncTime()
{
	bool result = timeClient.update();
	if (result)
	{
		unsigned long epoch = timeClient.getEpochTime();
		RtcDateTime date = RtcDateTime(epoch - 946684800UL); //ToDo: Move this constatn to an appropriate place

		Rtc.SetDateTime(date);
		_lastTimeSync = Rtc.GetDateTime().TotalSeconds();
		_clockMode = CM_CLOCK;
		_isRtcOk = true;
#ifdef CL_DEBUG
		Serial.print(F("Time: "));
		Serial.println(timeClient.getFormattedTime());
#endif
	}
	else
	{
#ifdef CL_DEBUG
		Serial.println(F("NTP error"));
#endif
	}
	return result;
}

void _connectToNetwork(bool fallbackToAp)
{
	Settings *sett = SettingsHelper::get();

	_connectionState = fallbackToAp ? CS_CONNECTING_W_AP : CS_CONNECTING;
	_connectionStart = millis();

	WiFi.disconnect();
	WiFi.begin(sett->wifiSSID, sett->wifiPwd);

	taskManager.StartTask(&taskCheckConnection);

#ifdef CL_DEBUG
	Serial.print(F("SSID: "));
	Serial.print(sett->wifiSSID);
	Serial.print(F(" PWD: "));
	Serial.println(sett->wifiPwd);
	Serial.println(F("Connecting"));
#endif
}

void _checkConnection(uint32_t deltaTime)
{
	unsigned long ms = millis();

	if (WiFi.isConnected())
	{
		_connectionState = CS_CONNECTED;
		taskManager.StopTask(&taskCheckConnection);
		return;
	}

#ifdef CL_DEBUG
	Serial.print('.');
#endif

	if (ms - _connectionStart > Cfg::wifiConnectionTimeout || ms < _connectionStart)
	{
		if (_connectionState == CS_CONNECTING_W_AP)
		{
			taskManager.StopTask(&taskCheckConnection);
			WiFi.disconnect();
			startSoftAP();
		}
	}
}

void _turnOfNetwork(uint32_t deltaTime)
{
	taskManager.StopTask(&taskTurnOfNetwork);
	if (_connectionState == CS_ACCESS_POINT)
	{
		WiFi.softAPdisconnect();
		_clockMode = _isRtcOk ? CM_CLOCK : CM_LOADING;
	}
	else
	{
		WiFi.disconnect();
	}
	_connectionState = CS_DISCONNECTED;
	stopWebServer();
}

void _delayedSyncTime(uint32_t deltaTime)
{
	if (syncTime())
	{
		taskManager.StopTask(&taskDelayedSyncTime);
		taskManager.StartTask(&taskTurnOfNetwork);
	}
}

void _scheduledSyncTime(uint32_t deltaTime)
{
	uint32_t noSync = Rtc.GetDateTime().TotalSeconds() - _lastTimeSync;
	if (noSync > _cfg->syncPeriod * 60)
	{
		taskScheduledSyncTime.setTimeInterval(5 * 60 * 1000);
		connectToNetwork(false);
	}
	else
	{
		WiFi.disconnect();
		_connectionState = CS_DISCONNECTED;
		taskScheduledSyncTime.setTimeInterval(_cfg->syncPeriod * 60 * 1000);
	}
}
