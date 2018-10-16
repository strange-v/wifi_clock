#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <FS.h>
#include <ArduinoOTA.h>
#include <Task.h>
#include <NTPClient.h>
#include <Wire.h>
#include <BH1750.h>
#include <RtcDS1307.h>
#include <Timezone.h>
#include <Adafruit_PWMServoDriver.h>
#include "SettingsHelper.h"
#include "Network.h"
#include "Cfg.h"
#include "Led.h"
#include "TimeSync.h"
#include "DisplayFunctionality.h"

WiFiEventHandler ehNetworkConnected, ehNetworkDisconnected;
TaskManager taskManager;
FunctionTask taskCheckConnection(_checkConnection, MsToTaskTime(5 *1000));
FunctionTask taskTurnOfAp(_turnOfAp, MsToTaskTime(2 * 60 * 1000));
FunctionTask taskSyncTime(_syncTime, MsToTaskTime(15 * 1000));

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);
BH1750 lightMeter;
RtcDS1307<TwoWire> Rtc(Wire);
Led led(&Wire, Cfg::pinOe);
Timezone* timezone;

Settings* _cfg;
ConnectionState _connectionState = CS_DISCONNECTED;
unsigned long _connectionStart = 0;
volatile byte _rtcPinState = LOW;
volatile byte _updateDisplay = 0;
unsigned long _testTime = 0;

void setup()
{
	Serial.begin(115200);
#ifdef _DEBUG
	delay(3000);
	printModuleInfo();
#endif

	WiFi.persistent(false);
	WiFi.mode(WIFI_STA);
	SPIFFS.begin();

	SettingsHelper::init(256);
	_cfg = SettingsHelper::get();
	
	ArduinoOTA.setPassword(_cfg->otaPwd);
	ArduinoOTA.begin();

	Wire.begin(Cfg::pinSda, Cfg::pinScl);
	Wire.setClock(400000);

	pinMode(Cfg::pinOe, OUTPUT);
	pinMode(Cfg::pinRtcInt, INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(Cfg::pinRtcInt), rtcInterrupt, CHANGE);

	bool lightMeterState = lightMeter.begin();
#ifdef _DEBUG
	if (lightMeterState)
		Serial.println(F("BH1750 initialized"));
	else
		Serial.println(F("Error initialising BH1750"));
#endif

	Rtc.Begin();
	Rtc.SetSquareWavePin(DS1307SquareWaveOut_1Hz);
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

	//ToDo: Read this from a config
	TimeChangeRule dst = { "DST", Last, Sun, Mar, 3, 180 };
	TimeChangeRule std = { "STD", Last, Sun, Oct, 4, 120 };
	timezone = new Timezone(dst, std);

	timeClient.begin();
	timeClient.setPoolServerName(_cfg->ntpUrl);

	connectToNetwork(true);

	led.begin();
}

void loop()
{
	ArduinoOTA.handle();
	yield();
	taskManager.Loop();
	yield();

	if (_updateDisplay > 0)
	{
		_updateDisplay--;
		updateDisplay();
	}

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
