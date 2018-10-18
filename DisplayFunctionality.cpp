#include "DisplayFunctionality.h"

void rtcInterrupt()
{
	_rtcPinState = digitalRead(Cfg::pinRtcInt);
	_updateDisplay++;
}

void updateDisplay()
{
	RtcDateTime now = Rtc.GetDateTime();
	SimpleTime checkTime{ now.Hour(), now.Minute() };

	uint16_t lux = lightMeter.readLightLevel();
	led.setPWM(lux > 0 ? lux : 1);
	
	if (_cfg->blinkColumn
		&& (!_cfg->doNotBlink || !isTimeBetween(checkTime, _cfg->dnbFrom, _cfg->dnbTo - 1)))
	{
		led.showColumn(static_cast<bool>(_rtcPinState));
	}
	else
	{
		led.showColumn(false);
	}

	led.showTime(now, _cfg->leadingZero);
}
