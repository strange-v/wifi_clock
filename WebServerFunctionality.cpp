#include "WebServerFunctionality.h"

void initWebServer()
{
	server.on("/", HTTP_GET, handleRoot);
	server.on("/config", HTTP_GET, getConfig);
	server.on("/config", HTTP_POST, saveConfig);
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

	StaticJsonBuffer<768> jb;
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
	startDST["week"] = dst.week;
	startDST["dow"] = dst.dow;
	startDST["month"] = dst.month;
	startDST["hour"] = dst.hour;
	startDST["offset"] = dst.offset;
	JsonObject& endDST = time.createNestedObject("endDST");
	TimeChangeRule std = _cfg->endDST;
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

	response.printTo(Serial);
	response.printTo(buffer);
	server.send(200, APPLICATION_JSON, buffer);
}

void saveConfig()
{
	if (!isAuthenticated())
		return;


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
