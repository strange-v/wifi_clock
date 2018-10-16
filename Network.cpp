#include "Network.h"

void connectToNetwork(bool fallbackToAp)
{
	ehNetworkConnected = WiFi.onStationModeGotIP([](const WiFiEventStationModeGotIP& event)
	{
		_connectionState = CS_CONNECTED;
#ifdef _DEBUG
		Serial.println();
		printNetworkInfo();
#endif
		taskManager.StartTask(&taskSyncTime);
	});

	_connectToNetwork(fallbackToAp);
}

void disconnectFromNetwork()
{
	WiFi.disconnect();
	_connectionState = CS_DISCONNECTING;
}

void startSoftAP()
{
#ifdef _DEBUG
	char ssid[6];
	memcpy(ssid, getSsid(), sizeof(ssid));
	
	Serial.println();
	Serial.print("Start AP: ");
	Serial.println(ssid);
#endif

	WiFi.softAP(ssid, Cfg::wifiApPassword, 6);
	taskManager.StartTask(&taskTurnOfAp);
	_connectionState = CS_ACCESS_POINT;

#ifdef _DEBUG
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("AP IP address: ");
	Serial.println(myIP);
#endif
}

void _connectToNetwork(bool fallbackToAp)
{
	Settings* sett = SettingsHelper::get();

	_connectionState = CS_CONNECTING_W_AP;
	_connectionStart = millis();

	WiFi.disconnect();
	WiFi.begin(sett->wifiSSID, sett->wifiPwd);

	taskManager.StartTask(&taskCheckConnection);

#ifdef _DEBUG
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

#ifdef _DEBUG
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

void _turnOfAp(uint32_t deltaTime)
{
	taskManager.StopTask(&taskTurnOfAp);
	WiFi.softAPdisconnect();
}