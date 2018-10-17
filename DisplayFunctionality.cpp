#include "DisplayFunctionality.h"

void rtcInterrupt()
{
	_rtcPinState = digitalRead(Cfg::pinRtcInt);
	_updateDisplay++;
}

void updateDisplay()
{
	RtcDateTime now = Rtc.GetDateTime();
	
	//ToDo: Add do not blick functionality

	led.showColumn(static_cast<bool>(_rtcPinState));
	led.showTime(now, _cfg->leadingZero);
}