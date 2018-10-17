#include "Led.h"

Led::Led(TwoWire* wire, byte pinOe)
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
	if (_showColumn == show)
		return;

	_showColumn = show;
	_pwm2->setPWM(15, 0, show ? _pwmValue : 0);
}

void Led::showDot(bool show)
{
	if (_showDot == show)
		return;

	_showDot = show;
	_pwm1->setPWM(15, 0, show ? _pwmValue : 0);
}

void Led::setPWM(uint16_t value)
{
	if (_pwmValue == value)
		return;

	_pwmValue = value;
	
	_showDigit(_digit[0], 0);
	_showDigit(_digit[1], 1);
	_showDigit(_digit[2], 2);
	_showDigit(_digit[3], 3);
	
	showDot(_showDot);
	showColumn(_showColumn);
}

void Led::_showDigit(byte value, byte position)
{
	if (value > 13)
		value = 11;

	if (_digit[position] == value)
		return;

	_digit[position] = value;
	Adafruit_PWMServoDriver* pwm = position > 1 ? _pwm2 : _pwm1 ;
	byte idx = position % 2 == 0 ? 0 : 8;

	for (size_t i = 0; i < 7; i++)
	{
		bool show = digitMap[value][i] == 1;
		pwm->setPWM(i + idx, 0, show ? _pwmValue : 0); //ToDo: Trye to use set pin
	}
}
