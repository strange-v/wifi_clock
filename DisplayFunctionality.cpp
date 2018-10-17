#include "DisplayFunctionality.h"

void rtcInterrupt()
{
	_rtcPinState = digitalRead(Cfg::pinRtcInt);
	_updateDisplay++;
}

void updateDisplay()
{
	RtcDateTime now = Rtc.GetDateTime();
	
	if (_cfg->blinkColumn
		&& (!_cfg->doNotBlink || !isTimeBetween(now, _cfg->dnbFrom, _cfg->dnbTo)))
	{
		led.showColumn(static_cast<bool>(_rtcPinState));
	}
	else
	{
		led.showColumn(false);
	}

	led.showTime(now, _cfg->leadingZero);
}

bool isTimeBetween(RtcDateTime time, SimpleTime start, SimpleTime end)
{
	SimpleTime checkTime{ time.Hour(), time.Minute() };
	
	if (start < end)
	{
		return checkTime >= start && checkTime <= end;
	}
	else
	{
		return checkTime >= start || checkTime <= end;
	}
}