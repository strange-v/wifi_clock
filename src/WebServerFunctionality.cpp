#include "WebServerFunctionality.h"

void initWebServer()
{
	server.on("/", HTTP_GET, handleRoot);
	server.on("/api/config", HTTP_GET, getConfig);
	server.on("/api/config", HTTP_POST, saveConfig);
	server.serveStatic("/static", SPIFFS, "/static");
	server.serveStatic("/assets", SPIFFS, "/assets");
	server.onNotFound(send404);
	server.begin();
}

void stopWebServer()
{
	server.stop();
}

void handleRoot()
{
	if (!isAuthenticated())
		return;

	taskTurnOfNetwork.setTimeInterval(5 * 60 * 1000ul);

	const char *path = "/index.html";

	if (!SPIFFS.exists(path))
	{
		send404();
		return;
	}

	File file = SPIFFS.open(path, "r");
	server.streamFile(file, TEXT_HTML);
	file.close();
}

void send404()
{
	server.send(404, TEXT_HTML, "Not Found");
}

void getConfig()
{
	if (!isAuthenticated())
		return;

	DynamicJsonBuffer jb(1042);
	char buffer[768];

	JsonObject &response = jb.createObject();
	response["code"] = 0;
	JsonObject &entity = response.createNestedObject("entity");

	JsonObject &info = entity.createNestedObject("info");
	info["id"] = ESP.getChipId();
	info["version"] = Cfg::version;

	JsonObject &system = entity.createNestedObject("system");
	system["wifiSSID"] = _cfg->wifiSSID;
	system["wifiPwd"] = "";
	system["authUser"] = _cfg->authUser;
	system["authPwd"] = "";
	system["otaPwd"] = "";

	JsonObject &time = entity.createNestedObject("time");
	time["ntpUrl"] = _cfg->ntpUrl;
	time["syncPeriod"] = _cfg->syncPeriod;
	time["useDST"] = _cfg->useDST;
	time["offset"] = _cfg->offset;
	JsonObject &startDST = time.createNestedObject("startDST");
	TimeChangeRule dst = _cfg->startDST;
	startDST["abbrev"] = dst.abbrev;
	startDST["week"] = dst.week;
	startDST["dow"] = dst.dow;
	startDST["month"] = dst.month;
	startDST["hour"] = dst.hour;
	startDST["offset"] = dst.offset;
	JsonObject &endDST = time.createNestedObject("endDST");
	TimeChangeRule std = _cfg->endDST;
	endDST["abbrev"] = std.abbrev;
	endDST["week"] = std.week;
	endDST["dow"] = std.dow;
	endDST["month"] = std.month;
	endDST["hour"] = std.hour;
	endDST["offset"] = std.offset;

	JsonObject &display = entity.createNestedObject("display");
	display["leadingZero"] = _cfg->leadingZero;
	display["blinkColumn"] = _cfg->blinkColumn;
	display["doNotBlink"] = _cfg->doNotBlink;
	display["minBrightness"] = _cfg->minBrightness;
	display["maxBrightness"] = _cfg->maxBrightness;
	JsonObject &dnbFrom = display.createNestedObject("dnbFrom");
	dnbFrom["hour"] = _cfg->dnbFrom.hour();
	dnbFrom["minute"] = _cfg->dnbFrom.minute();
	JsonObject &dnbTo = display.createNestedObject("dnbTo");
	dnbTo["hour"] = _cfg->dnbTo.hour();
	dnbTo["minute"] = _cfg->dnbTo.minute();

	response.printTo(buffer);
	server.send(200, APPLICATION_JSON, buffer);
}

void saveConfig()
{
	if (!isAuthenticated())
		return;

	DynamicJsonBuffer jb(1042);

	if (!server.hasArg("plain"))
	{
		server.send(200, APPLICATION_JSON, JSON_ERROR1);
		return;
	}

	JsonObject &root = jb.parseObject(server.arg("plain"));
	if (!root.success() || !root.containsKey("display") || !root.containsKey("system") || !root.containsKey("time"))
	{
		server.send(200, APPLICATION_JSON, JSON_ERROR2);
		return;
	}

	bool restartRequired = false;

	JsonObject &system = root["system"];
	const char *system_wifiSSID = system["wifiSSID"];
	const char *system_wifiPwd = system["wifiPwd"];
	const char *system_authUser = system["authUser"];
	const char *system_authPwd = system["authPwd"];
	const char *system_otaPwd = system["otaPwd"];
	if (system_wifiSSID != nullptr && !isStringEmpty(system_wifiSSID) && strcmp(system_wifiSSID, _cfg->wifiSSID) != 0)
	{
		SettingsHelper::setWifiSsid(system_wifiSSID);
		restartRequired = true;
	}
	if (system_wifiPwd != nullptr && !isStringEmpty(system_wifiPwd) && strcmp(system_wifiPwd, _cfg->wifiPwd) != 0)
	{
		SettingsHelper::setWifiPassword(system_wifiPwd);
		restartRequired = true;
	}
	if (system_authUser != nullptr && !isStringEmpty(system_authUser) && strcmp(system_authUser, _cfg->authUser) != 0)
	{
		SettingsHelper::setAuthUser(system_authUser);
		restartRequired = true;
	}
	if (system_authPwd != nullptr && !isStringEmpty(system_authPwd) && strcmp(system_authPwd, _cfg->authPwd) != 0)
	{
		SettingsHelper::setAuthPassword(system_authPwd);
		restartRequired = true;
	}
	if (system_otaPwd != nullptr && !isStringEmpty(system_otaPwd) && strcmp(system_otaPwd, _cfg->otaPwd) != 0)
	{
		SettingsHelper::setOtaPassword(system_otaPwd);
		restartRequired = true;
	}

	JsonObject &time = root["time"];
	const char *time_ntpUrl = time["ntpUrl"];
	unsigned int time_syncPeriod = time["syncPeriod"];
	bool time_useDST = time["useDST"];
	int offset = time["offset"];
	if (time_ntpUrl != nullptr && !isStringEmpty(time_ntpUrl) && strcmp(time_ntpUrl, _cfg->ntpUrl) != 0)
	{
		SettingsHelper::setNtpUrl(time_ntpUrl);
		timeClient.setPoolServerName(time_ntpUrl);
	}
	if (time_syncPeriod > 0 && time_syncPeriod != _cfg->syncPeriod)
	{
		SettingsHelper::setSyncPeriod(time_syncPeriod);
		taskScheduledSyncTime.setTimeInterval(_cfg->syncPeriod * 60 * 1000ul);
	}
	if (offset <= 14 * 60 || offset >= -12 * 60)
	{
		SettingsHelper::setOffset(offset);
	}
	else
	{
		offset = 0;
		SettingsHelper::setOffset(offset);
	}

	TimeChangeRule dst = {"dst", First, 1, 1, 1, offset};
	TimeChangeRule std = {"std", First, 1, 1, 1, offset};
	if (time_useDST)
	{
		JsonObject &time_startDST = time["startDST"];
		uint8_t time_startDST_week = time_startDST["week"];
		uint8_t time_startDST_dow = time_startDST["dow"];
		uint8_t time_startDST_month = time_startDST["month"];
		uint8_t time_startDST_hour = time_startDST["hour"];
		dst.week = time_startDST_week;
		dst.dow = time_startDST_dow;
		dst.month = time_startDST_month;
		dst.hour = time_startDST_hour;
		dst.offset = offset + 60;
		//dst = { "dst", time_startDST_week, time_startDST_dow, time_startDST_month, time_startDST_hour, offset + 60 };

		JsonObject &time_endDST = time["endDST"];
		uint8_t time_endDST_week = time_endDST["week"];
		uint8_t time_endDST_dow = time_endDST["dow"];
		uint8_t time_endDST_month = time_endDST["month"];
		uint8_t time_endDST_hour = time_endDST["hour"];
		std.week = time_endDST_week;
		std.dow = time_endDST_dow;
		std.month = time_endDST_month;
		std.hour = time_endDST_hour;
		std.offset = offset;
		//std = { "std", time_endDST_week, time_endDST_dow, time_endDST_month, time_endDST_hour, offset };
	}

	if (!SettingsHelper::isEqual(dst, _cfg->startDST) || !SettingsHelper::isEqual(std, _cfg->endDST))
	{
		SettingsHelper::setStartDst(dst);
		SettingsHelper::setEndDst(std);
		timezone->setRules(dst, std);
	}

	JsonObject &display = root["display"];
	bool display_leadingZero = display["leadingZero"];
	bool display_blinkColumn = display["blinkColumn"];
	bool display_doNotBlink = display["doNotBlink"];
	int display_minBrightness = display["minBrightness"];
	int display_maxBrightness = display["maxBrightness"];
	if (display_leadingZero != _cfg->leadingZero)
	{
		SettingsHelper::setLeadingZero(display_leadingZero);
	}
	if (display_blinkColumn != _cfg->blinkColumn)
	{
		SettingsHelper::setBlinkColumn(display_blinkColumn);
	}
	if (display_doNotBlink != _cfg->doNotBlink)
	{
		SettingsHelper::setDoNotBlink(display_doNotBlink);
	}
	if (display_minBrightness != _cfg->minBrightness)
	{
		SettingsHelper::setMinBrightness(display_minBrightness);
	}
	if (display_maxBrightness != _cfg->maxBrightness)
	{
		SettingsHelper::setMaxBrightness(display_maxBrightness);
	}

	uint8_t display_dnbFrom_hour = display["dnbFrom"]["hour"];
	uint8_t display_dnbFrom_minute = display["dnbFrom"]["minute"];
	SimpleTime dnbFrom(display_dnbFrom_hour, display_dnbFrom_minute);
	if (dnbFrom != _cfg->dnbFrom)
	{
		SettingsHelper::setDnbFrom(dnbFrom);
	}

	int display_dnbTo_hour = display["dnbTo"]["hour"];
	int display_dnbTo_minute = display["dnbTo"]["minute"];
	SimpleTime dnbTo(display_dnbTo_hour, display_dnbTo_minute);
	if (dnbTo != _cfg->dnbTo)
	{
		SettingsHelper::setDnbTo(dnbTo);
	}

	SettingsHelper::save();
	server.send(200, APPLICATION_JSON, JSON_SUCCESS);

	if (restartRequired)
	{
		delay(500);
		ESP.restart();
	}
}

bool isAuthenticated()
{
	if (server.authenticate(_cfg->authUser, _cfg->authPwd))
	{
		return true;
	}
	else
	{
		server.requestAuthentication();
		return false;
	}
}

bool isStringEmpty(const char *value)
{
	return value[0] == '\0';
}
