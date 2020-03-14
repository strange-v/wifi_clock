#include "DisplayFunctionality.h"

void ICACHE_RAM_ATTR rtcInterrupt()
{
	_rtcPinState = digitalRead(Cfg::pinRtcInt);
	_updateDisplay++;
}

void updateDisplay()
{
	if (_clockMode == CM_CLOCK)
	{
		RtcDateTime utc = Rtc.GetDateTime();
		time_t local = timezone->toLocal(utc + 946684800UL); //ToDo: Move this constatn to an appropriate place
		RtcDateTime now = RtcDateTime(local);
		SimpleTime checkTime(now.Hour(), now.Minute());

		uint16_t lux = lightMeter.readLightLevel();
		uint16_t pwm = map(lux, 0, 4000, _cfg->minBrightness, _cfg->maxBrightness);
		led.setPWM(pwm);

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
	else if (_clockMode == CM_CONFIG)
	{
		led.showSsid(ESP.getChipId() % 100);
	}
	else if (_clockMode == CM_LOADING)
	{
		led.spin();
	}
}
