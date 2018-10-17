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
	TimeChangeRule dst = { "DST", Last, Sun, Mar, 3, 180 };
	TimeChangeRule std = { "STD", Last, Sun, Oct, 4, 120 };

	setWifiSsid("SSID");
	setWifiPassword("PWD");
	setAuthUser("admin");
	setAuthPassword("admin");
	setOtaPassword("update");

	setNtpUrl("pool.ntp.org");
	setSyncPeriod(300);
	setUseDst(true);
	setStartDst(dst);
	setEndDst(std);

	setLeadingZero(true);
	setBlinkColumn(true);
	setDoNotBlink(false);
	setDnbFrom({ 0, 0 });
	setDnbTo({ 0, 0 });
	setMinBrightness(1);
	setMaxBrightness(4095);

	save();
}

#pragma region System
void SettingsHelper::setWifiSsid(char *value)
{
	makeStringSafe(value, sizeof(Settings::wifiSSID));
	if (strcmp(value, _settings.wifiSSID) != 0)
	{
		writeString(value, offsetof(Settings, Settings::wifiSSID), strlen(value) + 1);
	}
}

void SettingsHelper::setWifiPassword(char *value)
{
	makeStringSafe(value, sizeof(Settings::wifiPwd));
	if (strcmp(value, _settings.wifiPwd) != 0)
	{
		writeString(value, offsetof(Settings, Settings::wifiPwd), strlen(value) + 1);
	}
}

void SettingsHelper::setAuthUser(char *value)
{
	makeStringSafe(value, sizeof(Settings::authUser));
	if (strcmp(value, _settings.authUser) != 0)
	{
		writeString(value, offsetof(Settings, Settings::authUser), strlen(value) + 1);
	}
}

void SettingsHelper::setAuthPassword(char *value)
{
	makeStringSafe(value, sizeof(Settings::authPwd));
	if (strcmp(value, _settings.authPwd) != 0)
	{
		writeString(value, offsetof(Settings, Settings::authPwd), strlen(value) + 1);
	}
}

void SettingsHelper::setOtaPassword(char *value)
{
	makeStringSafe(value, sizeof(Settings::otaPwd));
	if (strcmp(value, _settings.otaPwd) != 0)
	{
		writeString(value, offsetof(Settings, Settings::otaPwd), strlen(value) + 1);
	}
}
#pragma endregion
#pragma region Time
void SettingsHelper::setNtpUrl(char *value)
{
	makeStringSafe(value, sizeof(Settings::ntpUrl));
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

void SettingsHelper::setStartDst(TimeChangeRule value)
{
	if (!isEqual(value, _settings.startDST))
	{
		EEPROM.put(offsetof(Settings, Settings::startDST), value);
	}
}

void SettingsHelper::setEndDst(TimeChangeRule value)
{
	if (isEqual(value, _settings.endDST))
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

void SettingsHelper::writeString(char *value, uint16_t addr, uint8_t len)
{
	for (int i = 0; i < len; i++)
	{
		EEPROM.write(addr + i, value[i]);
	}
}

void SettingsHelper::makeStringSafe(char *value, uint maxLen)
{
	uint len = strlen(value);
	if (len >= maxLen)
	{
		value[maxLen - 1] = '\0';
	}
}

bool SettingsHelper::isEqual(TimeChangeRule a, TimeChangeRule b)
{
	return strcmp(a.abbrev, b.abbrev)
		&& a.week == b.week
		&& a.dow == b.dow
		&& a.month == b.month
		&& a.hour == b.hour
		&& a.offset == b.offset;
}
