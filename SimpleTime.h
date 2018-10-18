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
