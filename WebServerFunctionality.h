#pragma once
#ifndef _WEBSERVERFUNCTIONALITY_h
#define _WEBSERVERFUNCTIONALITY_h

#include <Arduino.h>
#include <FS.h>
#include <Task.h>
#include <ESP8266WebServer.h>
#include <ArduinoJson.h>
#include "Module.h"
#include "WebServerConstants.h"
#include "SettingsHelper.h"
#include "Cfg.h"


void initWebServer();
void stopWebServer();
void handleRoot();
void send404();
void getConfig();
void saveConfig();
bool isAuthenticated();
bool isStringEmpty(const char* value);


extern ESP8266WebServer server;
extern TaskManager taskManager;
extern FunctionTask taskTurnOfNetwork;
extern Settings* _cfg;

#endif

