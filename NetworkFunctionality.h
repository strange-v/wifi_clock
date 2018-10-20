#pragma once
#ifndef _NETWORK_h
#define _NETWORK_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <NTPClient.h>
#include <RtcDS3231.h>
#include <Timezone.h>
#include <Task.h>
#include "Cfg.h"
#include "ClockMode.h"
#include "SettingsHelper.h"
#include "Module.h"
#include "WebServerFunctionality.h"

enum ConnectionState { CS_DISCONNECTED, CS_CONNECTING, CS_CONNECTING_W_AP, CS_CONNECTED, CS_STARTING_ACCESS_POINT, CS_ACCESS_POINT, CS_DISCONNECTING };

void connectToNetwork(bool fallbackToAp);
void startSoftAP();
bool syncTime();
void _connectToNetwork(bool fallbackToAp);
void _checkConnection(uint32_t deltaTime);
void _turnOfNetwork(uint32_t deltaTime);
void _delayedSyncTime(uint32_t deltaTime);
void _scheduledSyncTime(uint32_t deltaTime);

extern TaskManager taskManager;
extern FunctionTask taskCheckConnection, taskTurnOfNetwork, taskDelayedSyncTime, taskScheduledSyncTime;

extern WiFiEventHandler ehNetworkConnected, ehNetworkDisconnected;
extern ConnectionState _connectionState;
extern ClockMode _clockMode;
extern unsigned long _connectionStart;
extern uint32_t _lastTimeSync;
extern NTPClient timeClient;
extern RtcDS3231<TwoWire> Rtc;
extern Timezone* timezone;
extern Settings* _cfg;

#endif

