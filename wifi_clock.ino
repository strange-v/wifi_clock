#include <WiFiUdp.h>
#include <FS.h>
#include <ArduinoOTA.h>
#include <Task.h>
#include "SettingsHelper.h"
#include "Network.h"

TaskManager taskManager;
FunctionTask taskManageNetwork(_manageNetwork, MsToTaskTime(5 *1000));
FunctionTask taskTurnOfAp(_turnOfAp, MsToTaskTime(2 * 60 * 1000));

Settings* _cfg;
ConnectionState _connectionState = CS_DISCONNECTED;
unsigned long _connectionStart = 0;

void setup()
{
	Serial.begin(115200);
#ifdef _DEBUG
	delay(250);
	printModuleInfo();
#endif

	WiFi.persistent(false);
	WiFi.mode(WIFI_STA);
	SPIFFS.begin();

	SettingsHelper::init(256);
	_cfg = SettingsHelper::get();
	
	ArduinoOTA.setPassword(_cfg->otaPwd);
	ArduinoOTA.begin();

	SettingsHelper::setWifiSsid("SSID");
	SettingsHelper::save();
	
	connectToNetwork(true);
}

void loop()
{
	ArduinoOTA.handle();
	yield();
	taskManager.Loop();
	yield();
}
