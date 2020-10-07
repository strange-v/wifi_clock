#include "Led.h"

Led::Led(TwoWire *wire, byte pinOe)
{
	_pwm1 = new Adafruit_PWMServoDriver(&Wire, 0x40);
	_pwm2 = new Adafruit_PWMServoDriver(&Wire, 0x41);
	_pinOe = pinOe;
}

Led::~Led()
{
	delete _pwm1;
	delete _pwm2;
}

void Led::begin()
{
	_pwm1->begin();
	_pwm2->begin();

	_pwm1->reset();
	_pwm2->reset();

	_pwm1->setPWMFreq(1600);
	_pwm2->setPWMFreq(1600);

	digitalWrite(_pinOe, LOW);

	//ToDo: implement clear display
	showDot(false);
	showColumn(false);
}

void Led::showTime(RtcDateTime time, bool leadingZero)
{
	_showDigit(time.Hour() < 10 && !leadingZero ? 10 : time.Hour() / 10, 0);
	_showDigit(time.Hour() % 10, 1);
	_showDigit(time.Minute() / 10, 2);
	_showDigit(time.Minute() % 10, 3);
}

void Led::showSsid(byte id)
{
	_showDigit(12, 0);
	_showDigit(13, 1);
	_showDigit(id / 10, 2);
	_showDigit(id % 10, 3);
}

void Led::showColumn(bool show)
{
	_showColumn(show);
}

void Led::showDot(bool show)
{
	_showDot(show);
}

void Led::setPWM(uint16_t value)
{
	if (_pwmValue == value)
		return;

	_pwmValue = value;

	_showDigit(_digit[0], 0, true);
	_showDigit(_digit[1], 1, true);
	_showDigit(_digit[2], 2, true);
	_showDigit(_digit[3], 3, true);

	_showDot(_dotState, true);
	_showColumn(_columnState, true);
}

void Led::_showDigit(byte value, byte position, bool force)
{
	if (value > 13)
		value = 11;

	if (_digit[position] == value && !force)
		return;

	_digit[position] = value;
	Adafruit_PWMServoDriver *pwm = position > 1 ? _pwm2 : _pwm1;
	byte idx = position % 2 == 0 ? 0 : 8;

	for (size_t i = 0; i < 7; i++)
	{
		bool show = digitMap[value][i] == 1;
		pwm->setPin(i + idx, show ? _pwmValue : 0);
	}
}

void Led::_showColumn(bool show, bool force)
{
	if (_columnState == show && !force)
		return;

	_columnState = show;
	_pwm2->setPin(15, show ? _pwmValue : 0);
}

void Led::_showDot(bool show, bool force)
{
	if (_dotState == show && !force)
		return;

	_dotState = show;
	_pwm1->setPin(15, show ? _pwmValue : 0);
}

void Led::spin()
{
	for (byte p = 0; p < 4; p++)
	{
		Adafruit_PWMServoDriver *pwm = p > 1 ? _pwm2 : _pwm1;
		byte idx = p % 2 == 0 ? 0 : 8;
		for (size_t i = 0; i < 7; i++)
		{
			bool show = spinMap[_idx][p] == i;
			pwm->setPin(i + idx, show ? _pwmValue : 0);
		}
	}

	_idx++;
	if (_idx > 5)
		_idx = 0;
}
