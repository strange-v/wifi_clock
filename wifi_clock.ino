#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include "SettingsHelper.h"

void setup()
{
	Serial.begin(115200);
	delay(500);
	Serial.println();
	Serial.println(F("Starting..."));

	WiFi.persistent(false);
	WiFi.mode(WIFI_STA);

	SettingsHelper::init(256);

	SettingsHelper::setWifiSsid("SSID");
	SettingsHelper::setWifiPassword("PASSWORD");
	SettingsHelper::setAuthUser("admin");
	SettingsHelper::setAuthPassword("admin");
	SettingsHelper::setOtaPassword("update");

	SettingsHelper::setNtpUrl("pool.ntp.org");
	SettingsHelper::setSyncPeriod(300);
	SettingsHelper::setUseDst(true);
	SettingsHelper::setStartDst({ 0, 7, 3, 2, 120 });
	SettingsHelper::setEndDst({ 0, 6, 2, 1, 180 });

	SettingsHelper::setLeadingZero(true);
	SettingsHelper::setBlinkColumn(true);
	SettingsHelper::setDnbFrom({ 22, 45 });
	SettingsHelper::setDnbTo({ 8, 15 });
	SettingsHelper::setMinBrightness(1);
	SettingsHelper::setMaxBrightness(4095);

	SettingsHelper::save();
}

void loop()
{
	Settings* cfg = SettingsHelper::get();
	Serial.println("---System---");
	Serial.println(cfg->wifiSSID);
	Serial.println(cfg->wifiPwd);
	Serial.println(cfg->authUser);
	Serial.println(cfg->authPwd);
	Serial.println(cfg->authPwd);
	Serial.println("---Time---");
	Serial.println(cfg->ntpUrl);
	Serial.println(cfg->syncPeriod);
	Serial.println(cfg->useDST);
	Serial.print("week:");
	Serial.print(cfg->startDST.week);
	Serial.print(" dow:");
	Serial.print(cfg->startDST.dow);
	Serial.print(" month:");
	Serial.print(cfg->startDST.month);
	Serial.print(" hour:");
	Serial.print(cfg->startDST.hour);
	Serial.print(" offset:");
	Serial.println(cfg->startDST.offset);
	Serial.print("week:");
	Serial.print(cfg->endDST.week);
	Serial.print(" dow:");
	Serial.print(cfg->endDST.dow);
	Serial.print(" month:");
	Serial.print(cfg->endDST.month);
	Serial.print(" hour:");
	Serial.print(cfg->endDST.hour);
	Serial.print(" offset:");
	Serial.println(cfg->endDST.offset);
	Serial.println("---Display---");
	Serial.println(cfg->leadingZero);
	Serial.println(cfg->blinkColumn);
	Serial.println(cfg->doNotBlink);
	Serial.print("hour:");
	Serial.print(cfg->dnbFrom.hour);
	Serial.print("minute:");
	Serial.println(cfg->dnbFrom.minute);
	Serial.print("hour:");
	Serial.print(cfg->dnbTo.hour);
	Serial.print("minute:");
	Serial.println(cfg->dnbTo.minute);
	Serial.println(cfg->minBrightness);
	Serial.println(cfg->maxBrightness);

	delay(60000);
}
