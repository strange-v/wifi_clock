#include "Network.h"

void connectToNetwork(bool fallbackToAp)
{
	Settings* sett = SettingsHelper::get();
	_connectionState = CS_CONNECTING_W_AP;
	_connectionStart = millis();

	WiFi.mode(WIFI_STA);
	WiFi.begin(sett->wifiSSID, sett->wifiPwd);

	taskManager.StartTask(&taskManageNetwork);

#ifdef _DEBUG
	Serial.println(F("Connecting"));
#endif
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

void _manageNetwork(uint32_t deltaTime)
{
	unsigned long ms = millis();
//#ifdef _DEBUG
//	Serial.println(_connectionState);
//	Serial.println(WiFi.status());
//	Serial.println(ms - _connectionStart);
//	Serial.println();
//#endif

	switch (_connectionState)
	{
	case CS_CONNECTED:
	{
		_connectionState = (WiFi.status() == WL_CONNECTED)
			? CS_CONNECTED
			: CS_DISCONNECTED;

		if (_connectionState == CS_DISCONNECTED)
		{
			taskManager.StopTask(&taskManageNetwork);
		}
		break;
	}
	case CS_CONNECTING:
	case CS_CONNECTING_W_AP:
	{
#ifdef _DEBUG
		Serial.print('.');
#endif

		if (WiFi.status() == WL_CONNECTED)
		{
			_connectionState = CS_CONNECTED;

#ifdef _DEBUG
			Serial.println();
			printNetworkInfo();
#endif
			break;
		}

		if (ms - _connectionStart > Cfg::wifiConnectionTimeout || ms < _connectionStart)
		{
			_connectionState = _connectionState == CS_CONNECTING_W_AP ? CS_STARTING_ACCESS_POINT : CS_DISCONNECTING;
			WiFi.mode(WIFI_STA);
			WiFi.disconnect();
		}
		else
		{
			Settings* sett = SettingsHelper::get();
			WiFi.mode(WIFI_STA);
			WiFi.begin(sett->wifiSSID, sett->wifiPwd);
		}
		break;
	}
	case CS_STARTING_ACCESS_POINT:
	{
		if (WiFi.status() == WL_DISCONNECTED || WiFi.status() == WL_IDLE_STATUS)
		{
			startSoftAP();
		}
		break;
	}
	case CS_DISCONNECTING:
	{
		if (WiFi.status() == WL_DISCONNECTED || WiFi.status() == WL_IDLE_STATUS)
		{
			_connectionState = CS_DISCONNECTED;
		}
		break;
	}
	case CS_ACCESS_POINT:
	case CS_DISCONNECTED:
	default:
		break;
	}
}

void _turnOfAp(uint32_t deltaTime)
{
	WiFi.softAPdisconnect();
	connectToNetwork(true);
}