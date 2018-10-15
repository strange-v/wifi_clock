#pragma once
#include <Arduino.h>
#include <EEPROM.h>

const uint32_t SETTINFS_SIGNATURE = 312668204l;

struct DSTTime
{
	byte week;
	byte dow;
	byte month;
	byte hour;
	unsigned int offset;

	bool operator==(const DSTTime& b)
	{
		return (week == b.week && dow == b.dow && month == b.month && hour == b.hour && offset == b.offset);
	}
	bool operator!=(const DSTTime& b)
	{
		return !this->operator==(b);
	}
};
struct SimpleTime
{
	byte hour;
	byte minute;

	bool operator==(const SimpleTime& b)
	{
		return (hour == b.hour && minute == b.minute);
	}
	bool operator!=(const SimpleTime& b)
	{
		return !this->operator==(b);
	}
};
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
	DSTTime startDST;
	DSTTime endDST;
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
	static void setStartDst(DSTTime value);
	static void setEndDst(DSTTime value);

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

	static Settings _settings;
};


