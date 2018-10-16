#pragma once
#ifndef _TIMESYNC_h
#define _TIMESYNC_h

#include <Arduino.h>
#include <Wire.h>
#include <NTPClient.h>
#include <RtcDS1307.h>
#include <Timezone.h>
#include <Task.h>


bool syncTime();
void _syncTime(uint32_t deltaTime);

extern NTPClient timeClient;
extern RtcDS1307<TwoWire> Rtc;
extern Timezone* timezone;

extern TaskManager taskManager;
extern FunctionTask taskSyncTime;

#endif
