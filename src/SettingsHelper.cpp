#include "SettingsHelper.h"

Settings SettingsHelper::_settings;

void SettingsHelper::init(uint16_t length)
{
	EEPROM.begin(length);
	EEPROM.get(0, _settings);

	if (!isSignatureValid())
	{
		resetToDefaults();
	}
}

bool SettingsHelper::isSignatureValid()
{
	uint32_t signature;
	EEPROM.get(0, signature);

	return SETTINFS_SIGNATURE == signature;
}

Settings *SettingsHelper::get()
{
	return &_settings;
}

void SettingsHelper::save()
{
	if (!isSignatureValid())
	{
		EEPROM.put(0, SETTINFS_SIGNATURE);
	}

	EEPROM.commit();
	EEPROM.get(0, _settings);
}

void SettingsHelper::resetToDefaults()
{
	TimeChangeRule dst = {"DST", Last, Sun, Mar, 3, 0};
	TimeChangeRule std = {"STD", Last, Sun, Oct, 4, 0};

	setWifiSsid("SSID");
	setWifiPassword("PWD");
	setAuthUser("admin");
	setAuthPassword("admin");
	setOtaPassword("update");

	setNtpUrl("pool.ntp.org");
	setSyncPeriod(300);
	setUseDst(true);
	setOffset(120);
	setStartDst(dst);
	setEndDst(std);

	setLeadingZero(true);
	setBlinkColumn(true);
	setDoNotBlink(true);
	setDnbFrom(SimpleTime(22, 0));
	setDnbTo(SimpleTime(8, 0));
	setMinBrightness(1);
	setMaxBrightness(4000);

	save();
}

#pragma region System
void SettingsHelper::setWifiSsid(const char *value)
{
	const byte maxLen = sizeof(Settings::wifiSSID);
	char buffer[maxLen];

	makeSafeString(buffer, value, maxLen);
	if (strcmp(value, _settings.wifiSSID) != 0)
	{
		writeString(value, offsetof(Settings, Settings::wifiSSID), strlen(value) + 1);
	}
}

void SettingsHelper::setWifiPassword(const char *value)
{
	const byte maxLen = sizeof(Settings::wifiPwd);
	char buffer[maxLen];

	makeSafeString(buffer, value, maxLen);
	if (strcmp(value, _settings.wifiPwd) != 0)
	{
		writeString(value, offsetof(Settings, Settings::wifiPwd), strlen(value) + 1);
	}
}

void SettingsHelper::setAuthUser(const char *value)
{
	const byte maxLen = sizeof(Settings::authUser);
	char buffer[maxLen];

	makeSafeString(buffer, value, maxLen);
	if (strcmp(value, _settings.authUser) != 0)
	{
		writeString(value, offsetof(Settings, Settings::authUser), strlen(value) + 1);
	}
}

void SettingsHelper::setAuthPassword(const char *value)
{
	const byte maxLen = sizeof(Settings::authPwd);
	char buffer[maxLen];

	makeSafeString(buffer, value, maxLen);
	if (strcmp(value, _settings.authPwd) != 0)
	{
		writeString(value, offsetof(Settings, Settings::authPwd), strlen(value) + 1);
	}
}

void SettingsHelper::setOtaPassword(const char *value)
{
	const byte maxLen = sizeof(Settings::otaPwd);
	char buffer[maxLen];

	makeSafeString(buffer, value, maxLen);
	if (strcmp(value, _settings.otaPwd) != 0)
	{
		writeString(value, offsetof(Settings, Settings::otaPwd), strlen(value) + 1);
	}
}
#pragma endregion
#pragma region Time
void SettingsHelper::setNtpUrl(const char *value)
{
	const byte maxLen = sizeof(Settings::ntpUrl);
	char buffer[maxLen];

	makeSafeString(buffer, value, maxLen);
	if (strcmp(value, _settings.ntpUrl) != 0)
	{
		writeString(value, offsetof(Settings, Settings::ntpUrl), strlen(value) + 1);
	}
}

void SettingsHelper::setSyncPeriod(unsigned int value)
{
	if (value != _settings.syncPeriod)
	{
		EEPROM.put(offsetof(Settings, Settings::syncPeriod), value);
	}
}

void SettingsHelper::setUseDst(bool value)
{
	if (value != _settings.useDST)
	{
		EEPROM.put(offsetof(Settings, Settings::useDST), value);
	}
}

void SettingsHelper::setOffset(int value)
{
	if (value != _settings.offset)
	{
		EEPROM.put(offsetof(Settings, Settings::offset), value);
	}
}

void SettingsHelper::setStartDst(TimeChangeRule value)
{
	if (!isEqual(value, _settings.startDST))
	{
		EEPROM.put(offsetof(Settings, Settings::startDST), value);
	}
}

void SettingsHelper::setEndDst(TimeChangeRule value)
{
	if (!isEqual(value, _settings.endDST))
	{
		EEPROM.put(offsetof(Settings, Settings::endDST), value);
	}
}
#pragma endregion
#pragma region Display
void SettingsHelper::setLeadingZero(bool value)
{
	if (value != _settings.leadingZero)
	{
		EEPROM.put(offsetof(Settings, Settings::leadingZero), value);
	}
}

void SettingsHelper::setBlinkColumn(bool value)
{
	if (value != _settings.blinkColumn)
	{
		EEPROM.put(offsetof(Settings, Settings::blinkColumn), value);
	}
}

void SettingsHelper::setDoNotBlink(bool value)
{
	if (value != _settings.doNotBlink)
	{
		EEPROM.put(offsetof(Settings, Settings::doNotBlink), value);
	}
}

void SettingsHelper::setDnbFrom(SimpleTime value)
{
	if (value != _settings.dnbFrom)
	{
		EEPROM.put(offsetof(Settings, Settings::dnbFrom), value);
	}
}

void SettingsHelper::setDnbTo(SimpleTime value)
{
	if (value != _settings.dnbTo)
	{
		EEPROM.put(offsetof(Settings, Settings::dnbTo), value);
	}
}

void SettingsHelper::setMinBrightness(unsigned int value)
{
	if (value != _settings.minBrightness)
	{
		EEPROM.put(offsetof(Settings, Settings::minBrightness), value);
	}
}

void SettingsHelper::setMaxBrightness(unsigned int value)
{
	if (value != _settings.maxBrightness)
	{
		EEPROM.put(offsetof(Settings, Settings::maxBrightness), value);
	}
}
#pragma endregion

void SettingsHelper::writeString(const char *value, uint16_t addr, uint8_t len)
{
	for (int i = 0; i < len; i++)
	{
		EEPROM.write(addr + i, value[i]);
	}
}

void SettingsHelper::makeSafeString(char *buffer, const char *value, uint maxLen)
{
	memcpy(buffer, value, maxLen);

	uint len = strlen(buffer);
	if (len >= maxLen)
	{
		buffer[maxLen - 1] = '\0';
	}
}

bool SettingsHelper::isEqual(TimeChangeRule a, TimeChangeRule b)
{
	return strcmp(a.abbrev, b.abbrev) && a.week == b.week && a.dow == b.dow && a.month == b.month && a.hour == b.hour && a.offset == b.offset;
}
