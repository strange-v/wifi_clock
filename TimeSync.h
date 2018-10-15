#pragma once
#ifndef _TIMESYNC_h
#define _TIMESYNC_h

#include <Arduino.h>
#include <Wire.h>
#include <NTPClient.h>
#include <RtcDS1307.h>


void syncTime();

extern NTPClient timeClient;
extern RtcDS1307<TwoWire> Rtc;

#endif
