#include "DisplayFunctionality.h"

void rtcInterrupt()
{
	_rtcPinState = digitalRead(Cfg::pinRtcInt);
	_updateDisplay++;
}

void updateDisplay()
{
	led.showColumn(static_cast<bool>(_rtcPinState));

	RtcDateTime now = Rtc.GetDateTime();
	led.showTime(now.Hour(), now.Minute());
}