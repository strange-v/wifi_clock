#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <RtcDS3231.h>

#pragma once
class Led
{
public:
	Led(TwoWire* wire, byte pinOe);
	~Led();
	void begin();
	void showTime(RtcDateTime time, bool leadingZero);
	void showSsid(byte id);
	void showColumn(bool show);
	void showDot(bool show);
	void setPWM(uint16_t value);
private:
	void _showDigit(byte value, byte position);

	Adafruit_PWMServoDriver* _pwm1;
	Adafruit_PWMServoDriver* _pwm2;
	byte _pinOe;
	uint16_t _pwmValue = 100;
	byte _digit[4]{10, 10, 10, 10};
	bool _showColumn;
	bool _showDot;

	const byte digitMap[14][7] = {
		{1, 1, 1, 1, 1, 1, 0}, //0
		{0, 1, 1, 0, 0, 0, 0}, //1
		{1, 1, 0, 1, 1, 0, 1}, //2
		{1, 1, 1, 1, 0, 0, 1}, //3
		{0, 1, 1, 0, 0, 1, 1}, //4
		{1, 0, 1, 1, 0, 1, 1}, //5
		{1, 0, 1, 1, 1, 1, 1}, //6
		{1, 1, 1, 0, 0, 0, 0}, //7
		{1, 1, 1, 1, 1, 1, 1}, //8
		{1, 1, 1, 1, 0, 1, 1}, //9

		{0, 0, 0, 0, 0, 0, 0}, //empty
		{0, 0, 0, 0, 0, 0, 1}, //-
		{1, 0, 0, 1, 1, 1, 0}, //C
		{0, 0, 0, 1, 1, 1, 0}, //L
	};
};

