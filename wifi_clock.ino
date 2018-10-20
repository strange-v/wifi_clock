#include "WebServerFunctionality.h"
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <ArduinoOTA.h>
#include <Task.h>
#include <NTPClient.h>
#include <Wire.h>
#include <BH1750.h>
#include <RtcDS3231.h>
#include <Timezone.h>
#include <Adafruit_PWMServoDriver.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "ClockMode.h"
#include "SettingsHelper.h"
#include "Cfg.h"
#include "Led.h"
#include "NetworkFunctionality.h"
#include "DisplayFunctionality.h"

TaskManager taskManager;
FunctionTask taskCheckConnection(_checkConnection, MsToTaskTime(5 *1000));
FunctionTask taskTurnOfNetwork(_turnOfNetwork, MsToTaskTime(2 * 60 * 1000));
FunctionTask taskDelayedSyncTime(_delayedSyncTime, MsToTaskTime(15 * 1000));
FunctionTask taskScheduledSyncTime(_scheduledSyncTime, MsToTaskTime(5 * 60 * 60 * 1000ul));

WiFiEventHandler ehNetworkConnected, ehNetworkDisconnected;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
BH1750 lightMeter;
RtcDS3231<TwoWire> Rtc(Wire);
Led led(&Wire, Cfg::pinOe);
Timezone* timezone;
ESP8266WebServer server(80);

Settings* _cfg;
ConnectionState _connectionState = CS_DISCONNECTED;
ClockMode _clockMode = CM_LOADING;
unsigned long _connectionStart = 0;
volatile byte _rtcPinState = LOW;
volatile byte _updateDisplay = 0;
uint32_t _lastTimeSync = 0;
unsigned long _testTime = 0;

void setup()
{
	Serial.begin(115200);
#ifdef _DEBUG
	delay(3000);
	printModuleInfo();
#endif

	pinMode(Cfg::pinOe, OUTPUT);
	pinMode(Cfg::pinRtcInt, INPUT_PULLUP);

	WiFi.persistent(false);
	WiFi.mode(WIFI_STA);
	SPIFFS.begin();

	SettingsHelper::init(256);
	_cfg = SettingsHelper::get();
	
	ArduinoOTA.setPassword(_cfg->otaPwd);
	ArduinoOTA.begin();

	Wire.begin(Cfg::pinSda, Cfg::pinScl);
	Wire.setClock(400000);

	led.begin();

	bool lightMeterState = lightMeter.begin();
#ifdef _DEBUG
	if (lightMeterState)
		Serial.println(F("BH1750 initialized"));
	else
		Serial.println(F("Error initialising BH1750"));
#endif

	Rtc.Begin();
	Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeClock);
	Rtc.SetSquareWavePinClockFrequency(DS3231SquareWaveClock_1Hz);
	if (Rtc.IsDateTimeValid() && Rtc.GetIsRunning())
	{
		_clockMode = CM_CLOCK;
	}
	else
	{
		if (!Rtc.IsDateTimeValid())
		{
#ifdef _DEBUG
			Serial.println(F("RTC lost confidence in the DateTime!"));
#endif
			Rtc.SetDateTime(0);
		}

		if (!Rtc.GetIsRunning())
		{
#ifdef _DEBUG
			Serial.println(F("RTC was not actively running, starting now"));
#endif
			Rtc.SetIsRunning(true);
		}
	}	

	timezone = new Timezone(_cfg->startDST, _cfg->endDST);
	timeClient.begin();
	timeClient.setPoolServerName(_cfg->ntpUrl);

	taskScheduledSyncTime.setTimeInterval(_cfg->syncPeriod * 60 * 1000ul);
	taskManager.StartTask(&taskScheduledSyncTime);

	attachInterrupt(digitalPinToInterrupt(Cfg::pinRtcInt), rtcInterrupt, CHANGE);

	connectToNetwork(true);
}

void loop()
{
	ArduinoOTA.handle();
	yield();
	taskManager.Loop();
	yield();
	server.handleClient();
	yield();

	if (_updateDisplay > 0)
	{
		_updateDisplay--;
		updateDisplay();
	}
#ifdef _DEBUG
	unsigned long ms = millis();
	if (ms - _testTime > 10000 || ms < _testTime)
	{
		_testTime = ms;

		uint16_t lux = lightMeter.readLightLevel();
		Serial.print("Light: ");
		Serial.print(lux);
		Serial.println(" lx");

		RtcDateTime now = Rtc.GetDateTime();
		printDateTime(now);
		Serial.println();
	}
#endif
}

#define countof(a) (sizeof(a) / sizeof(a[0]))

void printDateTime(const RtcDateTime& dt)
{
	char datestring[20];

	snprintf_P(datestring,
		countof(datestring),
		PSTR("%02u/%02u/%04u %02u:%02u:%02u"),
		dt.Month(),
		dt.Day(),
		dt.Year(),
		dt.Hour(),
		dt.Minute(),
		dt.Second());
	Serial.print(datestring);
}
