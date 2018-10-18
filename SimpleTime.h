#pragma once
#include <Arduino.h>

struct SimpleTime
{
	uint8_t hour;
	uint8_t minute;

	friend bool operator==(const SimpleTime& l, const SimpleTime& r)
	{
		return l.hour == r.hour && l.minute == r.minute;
	}

	friend bool operator!=(const SimpleTime& l, const SimpleTime& r)
	{
		return !(l == r);
	}

	friend bool operator>(const SimpleTime& l, const SimpleTime& r)
	{
		return l.hour > r.hour || (l.hour == r.hour && l.minute > r.minute);
	}

	friend bool operator<(const SimpleTime& l, const SimpleTime& r)
	{
		return !(l > r) && l != r;
	}

	friend bool operator>=(const SimpleTime& l, const SimpleTime& r)
	{
		return l == r || l > r;
	}

	friend bool operator<=(const SimpleTime& l, const SimpleTime& r)
	{
		return l == r || l < r;
	}

	friend SimpleTime operator-(SimpleTime l, const uint16_t value)
	{
		uint16_t minutes = l.hour * 60 + l.minute;

		if (value <= minutes)
		{
			minutes -= value;
		}
		else
		{
			minutes = 1440 - (minutes - value);
		}

		l.hour = minutes / 60;
		l.minute = minutes % 60;

		return l;
	}

	friend SimpleTime operator+(SimpleTime l, const uint16_t value)
	{
		uint32_t minutes = l.hour * 60 + l.minute;
		minutes += value;
		minutes = minutes % 1440;

		l.hour = minutes / 60;
		l.minute = minutes % 60;

		return l;
	}
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
