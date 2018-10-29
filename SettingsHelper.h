#pragma once
#include <Arduino.h>
#include <EEPROM.h>
#include <Timezone.h>
#include "SimpleTime.h"

const uint32_t SETTINFS_SIGNATURE = 312647204l;

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
	
	static void setWifiSsid(const char *value);
	static void setWifiPassword(const char *value);
	static void setAuthUser(const char *value);
	static void setAuthPassword(const char *value);
	static void setOtaPassword(const char *value);
	
	static void setNtpUrl(const char *value);
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

	static bool isEqual(TimeChangeRule a, TimeChangeRule b);
private:
	static void writeString(const char *value, uint16_t addr, uint8_t len);
	static void makeSafeString(char *buffer, const char *value, uint maxLen);

	static Settings _settings;
};


