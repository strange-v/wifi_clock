#include "Module.h"

void printModuleInfo()
{
	Serial.println();
	Serial.println(F("WiFi Clock"));
	Serial.print(F("Id: "));
	Serial.print(ESP.getChipId());
	Serial.print(F(" v. "));
	Serial.println(Cfg::version);
	Serial.println();
}

void printNetworkInfo()
{
	Serial.println(F("Connected"));
	Serial.print(F("IP: "));
	Serial.println(WiFi.localIP());
	Serial.print(F("MAC: "));
	Serial.println(WiFi.macAddress());
}

void restartModule()
{
	ESP.restart();
}

char *getSsid()
{
	char ssid[6];

	sprintf(ssid, "%s%d", Cfg::wifiApSsid, ESP.getChipId() % 100);
	return ssid;
}
