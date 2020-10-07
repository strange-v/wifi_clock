#pragma once
#include <Arduino.h>

struct SimpleTime
{
	SimpleTime(uint8_t hour, uint8_t minute)
	{
		_value = hour * 60 + minute;
	}

	SimpleTime(uint16_t minutes)
	{
		_value = minutes;
	}

	SimpleTime()
	{
		_value = 0;
	}

	bool operator==(const SimpleTime &r)
	{
		return _value == r._value;
	}
	bool operator!=(const SimpleTime &r)
	{
		return _value != r._value;
	}

	bool operator>(const SimpleTime &r)
	{
		return _value > r._value;
	}
	bool operator<(const SimpleTime &r)
	{
		return _value < r._value;
	}

	bool operator>=(const SimpleTime &r)
	{
		return _value >= r._value;
	}
	bool operator<=(const SimpleTime &r)
	{
		return _value <= r._value;
	}

	SimpleTime operator-(const uint16_t value)
	{
		SimpleTime result(0);

		if (value <= _value)
		{
			result._value = this->_value - value;
		}
		else
		{
			result._value = 1440 - (this->_value - value);
		}

		return result;
	}
	SimpleTime operator+(const uint16_t value)
	{
		SimpleTime result(0);

		result._value = (this->_value + value) % 1440;

		return result;
	}

	uint8_t hour()
	{
		return _value / 60;
	}

	uint8_t minute()
	{
		return _value % 60;
	}

	uint16_t getMinutes()
	{
		return _value;
	}

private:
	uint16_t _value;
};

inline bool isTimeBetween(SimpleTime time, SimpleTime start, SimpleTime end)
{
	if (start < end)
	{
		return time >= start && time <= end;
	}
	else
	{
		return time >= start || time <= end;
	}
}