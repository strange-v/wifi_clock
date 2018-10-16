#pragma once
#ifndef _DISPLAYFUNCTIONALITY_h
#define _DISPLAYFUNCTIONALITY_h

#include <Arduino.h>
#include <Wire.h>
#include <RtcDS1307.h>
#include "Led.h"
#include "Cfg.h"

void rtcInterrupt();
void updateDisplay();

extern RtcDS1307<TwoWire> Rtc;
extern Led led;

extern volatile byte _rtcPinState;
extern volatile byte _updateDisplay;

#endif

