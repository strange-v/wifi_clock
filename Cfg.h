#pragma once
#include <Arduino.h>

namespace Cfg
{
	const char wifiApSsid[] = "CL:";
	const char wifiApPassword[] = "123456789";
	const unsigned long wifiConnectionTimeout = 1 * 60 * 1000;
	const unsigned long wifiAPTimeout = 1 * 60 * 1000;

	const byte pinSda = 4;
	const byte pinScl = 5;
	const byte pinOe = 12;

	const char version[] = "1.0.0";
}
