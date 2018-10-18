#pragma once
#ifndef _DISPLAYFUNCTIONALITY_h
#define _DISPLAYFUNCTIONALITY_h

#include <Arduino.h>
#include <Wire.h>
#include <RtcDS3231.h>
#include <BH1750.h>
#include <TimeLib.h>
#include "SettingsHelper.h"
#include "Led.h"
#include "Cfg.h"

void rtcInterrupt();
void updateDisplay();

extern RtcDS3231<TwoWire> Rtc;
extern BH1750 lightMeter;
extern Led led;

extern Settings* _cfg;
extern volatile byte _rtcPinState;
extern volatile byte _updateDisplay;

#endif

