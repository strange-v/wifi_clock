#pragma once
#ifndef _NETWORK_h
#define _NETWORK_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Task.h>
#include "Cfg.h"
#include "SettingsHelper.h"
#include "Module.h"

enum ConnectionState { CS_DISCONNECTED, CS_CONNECTING, CS_CONNECTING_W_AP, CS_CONNECTED, CS_STARTING_ACCESS_POINT, CS_ACCESS_POINT, CS_DISCONNECTING };

void connectToNetwork(bool fallbackToAp);
void disconnectFromNetwork();
void startSoftAP();
void _manageNetwork(uint32_t deltaTime);
void _turnOfAp(uint32_t deltaTime);

extern TaskManager taskManager;
extern FunctionTask taskManageNetwork;
extern FunctionTask taskTurnOfAp;

extern ConnectionState _connectionState;
extern unsigned long _connectionStart;

#endif

