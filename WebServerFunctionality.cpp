#include "WebServerFunctionality.h"

void initWebServer()
{
	server.on("/", HTTP_GET, handleRoot);
	server.on("/api/config", HTTP_GET, getConfig);
	server.on("/api/config", HTTP_POST, saveConfig);
	server.onNotFound(send404);
	server.begin();
}

void stopWebServer()
{
	server.stop();
}

void handleRoot() {
	if (!isAuthenticated())
		return;

	taskTurnOfNetwork.setTimeInterval(5 * 60 * 1000ul);

	const char* path = "/index.html";

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

	JsonObject& response = jb.createObject();
	response["code"] = 0;
	JsonObject& entity = response.createNestedObject("entity");

	JsonObject& info = entity.createNestedObject("info");
	info["id"] = ESP.getChipId();
	info["version"] = Cfg::version;

	JsonObject& system = entity.createNestedObject("system");
	system["wifiSSID"] = _cfg->wifiSSID;
	system["wifiPwd"] = "     ";
	system["authUser"] = _cfg->authUser;
	system["authPwd"] = "     ";
	system["otaPwd"] = "     ";

	JsonObject& time = entity.createNestedObject("time");
	time["ntpUrl"] = _cfg->ntpUrl;
	time["syncPeriod"] = _cfg->syncPeriod;
	time["useDST"] = _cfg->useDST;
	JsonObject& startDST = time.createNestedObject("startDST");
	TimeChangeRule dst = _cfg->startDST;
	startDST["abbrev"] = dst.abbrev;
	startDST["week"] = dst.week;
	startDST["dow"] = dst.dow;
	startDST["month"] = dst.month;
	startDST["hour"] = dst.hour;
	startDST["offset"] = dst.offset;
	JsonObject& endDST = time.createNestedObject("endDST");
	TimeChangeRule std = _cfg->endDST;
	endDST["abbrev"] = std.abbrev;
	endDST["week"] = std.week;
	endDST["dow"] = std.dow;
	endDST["month"] = std.month;
	endDST["hour"] = std.hour;
	endDST["offset"] = std.offset;

	JsonObject& display = entity.createNestedObject("display");
	display["leadingZero"] = _cfg->leadingZero;
	display["blinkColumn"] = _cfg->blinkColumn;
	display["doNotBlink"] = _cfg->doNotBlink;
	display["minBrightness"] = _cfg->minBrightness;
	display["maxBrightness"] = _cfg->maxBrightness;
	JsonObject& dnbFrom = display.createNestedObject("dnbFrom");
	dnbFrom["hour"] = _cfg->dnbFrom.hour();
	dnbFrom["minute"] = _cfg->dnbFrom.minute();
	JsonObject& dnbTo = display.createNestedObject("dnbTo");
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

	JsonObject& data = jb.parseObject(server.arg("plain"));
	if (!data.success() || !data.containsKey("display") || !data.containsKey("system") || !data.containsKey("time"))
	{
		server.send(200, APPLICATION_JSON, JSON_ERROR2);
		return;
	}

	JsonObject& system = data["system"];
	JsonObject& time = data["time"];
	JsonObject& display = data["display"];

	//system
	const char* wifiSSID = system["wifiSSID"];
	if (wifiSSID != nullptr && !isStringEmpty(wifiSSID))
		SettingsHelper::setWifiSsid(wifiSSID);
	const char* wifiPwd = system["wifiPwd"];
	if (wifiPwd != nullptr && !isStringEmpty(wifiPwd))
		SettingsHelper::setWifiPassword(wifiPwd);
	const char* authUser = system["authUser"];
	if (authUser != nullptr && !isStringEmpty(authUser))
		SettingsHelper::setAuthUser(authUser);
	const char* authPwd = system["authPwd"];
	if (authPwd != nullptr && !isStringEmpty(authPwd))
		SettingsHelper::setAuthPassword(authPwd);
	const char* otaPwd = system["otaPwd"];
	if (otaPwd != nullptr && !isStringEmpty(otaPwd))
		SettingsHelper::setOtaPassword(otaPwd);

	//time
	const char* ntpUrl = time["ntpUrl"];
	if (ntpUrl != nullptr && !isStringEmpty(ntpUrl))
		SettingsHelper::setNtpUrl(ntpUrl);
	if (time.containsKey("syncPeriod"))
		SettingsHelper::setSyncPeriod(time["syncPeriod"].as<int>());
	if (time.containsKey("useDST"))
		SettingsHelper::setUseDst(time["useDST"].as<bool>());
	if (time.containsKey("startDST"))
	{
		JsonObject& data = time["startDST"];
		TimeChangeRule rule;
		strcpy(rule.abbrev, "dst");
		rule.week = data["week"];
		rule.dow = data["dow"];
		rule.month = data["month"];
		rule.hour = data["hour"];
		rule.offset = data["offset"];

		SettingsHelper::setStartDst(rule);
	}
	if (time.containsKey("endDST"))
	{
		JsonObject& data = time["endDST"];
		TimeChangeRule rule;
		strcpy(rule.abbrev, "std");
		rule.week = data["week"];
		rule.dow = data["dow"];
		rule.month = data["month"];
		rule.hour = data["hour"];
		rule.offset = data["offset"];

		SettingsHelper::setEndDst(rule);
	}

	//display
	if (display.containsKey("leadingZero"))
		SettingsHelper::setLeadingZero(display["leadingZero"].as<bool>());
	if (display.containsKey("blinkColumn"))
		SettingsHelper::setBlinkColumn(display["blinkColumn"].as<bool>());
	if (display.containsKey("doNotBlink"))
		SettingsHelper::setDoNotBlink(display["doNotBlink"].as<bool>());
	if (display.containsKey("minBrightness"))
		SettingsHelper::setMinBrightness(display["minBrightness"].as<unsigned int>());
	if (display.containsKey("maxBrightness"))
		SettingsHelper::setMaxBrightness(display["maxBrightness"].as<unsigned int>());
	if (display.containsKey("dnbFrom"))
	{
		JsonObject& data = display["dnbFrom"];
		SettingsHelper::setDnbFrom(SimpleTime(data["hour"].as<uint8_t>(), data["minute"].as<uint8_t>()));
	}
	if (display.containsKey("dnbTo"))
	{
		JsonObject& data = display["dnbTo"];
		SettingsHelper::setDnbTo(SimpleTime(data["hour"].as<uint8_t>(), data["minute"].as<uint8_t>()));
	}

	SettingsHelper::save();
	server.send(200, APPLICATION_JSON, JSON_SUCCESS);
	
	delay(500);
	ESP.restart();
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

bool isStringEmpty(const char* value)
{
	return value[0] == '\0';
}
