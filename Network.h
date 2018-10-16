#pragma once
#ifndef _NETWORK_h
#define _NETWORK_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Task.h>
#include "Cfg.h"
#include "SettingsHelper.h"
#include "Module.h"
#include "TimeSync.h"

enum ConnectionState { CS_DISCONNECTED, CS_CONNECTING, CS_CONNECTING_W_AP, CS_CONNECTED, CS_STARTING_ACCESS_POINT, CS_ACCESS_POINT, CS_DISCONNECTING };

void connectToNetwork(bool fallbackToAp);
void disconnectFromNetwork();
void startSoftAP();
void _connectToNetwork(bool fallbackToAp);
void _checkConnection(uint32_t deltaTime);
void _turnOfAp(uint32_t deltaTime);

extern WiFiEventHandler ehNetworkConnected;
extern WiFiEventHandler ehNetworkDisconnected;

extern TaskManager taskManager;
extern FunctionTask taskCheckConnection;
extern FunctionTask taskTurnOfAp;
extern FunctionTask taskSyncTime;

extern ConnectionState _connectionState;
extern unsigned long _connectionStart;

#endif

