#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <Timezone.h>
#include "SimpleTime.h"

const uint32_t SETTINFS_SIGNATURE = 312637204l;

struct Settings
{
	uint32_t signature;
#pragma region System
	char wifiSSID[32];
	char wifiPwd[32];
	char authUser[16];
	char authPwd[16];
	char otaPwd[16];
#pragma endregion
#pragma region Time
	char ntpUrl[32];
	unsigned int syncPeriod;
	bool useDST;
	TimeChangeRule startDST;
	TimeChangeRule endDST;
#pragma endregion
#pragma region Display
	bool leadingZero;
	bool blinkColumn;
	bool doNotBlink;
	SimpleTime dnbFrom;
	SimpleTime dnbTo;
	unsigned int minBrightness;
	unsigned int maxBrightness;
#pragma endregion
};

class SettingsHelper
{
public:
	static void init(uint16_t lenth);
	static bool isSignatureValid();
	static Settings* get();
	static void save();
	static void resetToDefaults();
	
	static void setWifiSsid(char *value);
	static void setWifiPassword(char *value);
	static void setAuthUser(char *value);
	static void setAuthPassword(char *value);
	static void setOtaPassword(char *value);
	
	static void setNtpUrl(char *value);
	static void setSyncPeriod(unsigned int value);
	static void setUseDst(bool value);
	static void setStartDst(TimeChangeRule value);
	static void setEndDst(TimeChangeRule value);

	static void setLeadingZero(bool value);
	static void setBlinkColumn(bool value);
	static void setDoNotBlink(bool value);
	static void setDnbFrom(SimpleTime value);
	static void setDnbTo(SimpleTime value);
	static void setMinBrightness(unsigned int value);
	static void setMaxBrightness(unsigned int value);
private:
	static void writeString(char *value, uint16_t addr, uint8_t len);
	static void makeStringSafe(char *value, uint maxLen);
	static bool isEqual(TimeChangeRule a, TimeChangeRule b);

	static Settings _settings;
};


