#pragma once
#ifndef _MODULE_h
#define _MODULE_h

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "Cfg.h"

void printModuleInfo();
void printNetworkInfo();
void restartModule();
char *getSsid();
#endif
