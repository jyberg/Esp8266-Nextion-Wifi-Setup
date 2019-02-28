/**
 * @file main.cpp
 *
 * Esp8266 Wifi / SoftAP setup via Noextion UI
 *
 * @author Jyrki Berg 2/17/2019 (https://github.com/jyberg)
 * 
 * @copyright 
 * Copyright (C) 2019 Jyrki Berg \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */



// for debug printing to Serial0, by default Serial uses Serial 1 (Swapped ports)
//#define DEBUG


#include <Arduino.h>
#include "debugPrint.h"

#include "globalSettings.h"
#include "wifi.h"
#include "nextionUi.h"

#include <HardwareSerial.h>
#include <time.h>

using namespace nextionUi;
using namespace wifi;


void setup() 
{
	Serial.begin(9600);
	delay(100);

	DisableWifi();
	
	if(LoadGlobalSettings())
	{
		DebugPrintLn("eeprom data valid")
	}
	else
	{
		memset(&gSettings, 0, sizeof(gSettings));
		gSettings.version = version;
		sprintf(gSettings.hostname, "exp8266Hostname");
		sprintf(gSettings.apSsid, "esp8266");
		gSettings.apChannel = 11;
		gSettings.apOn = true;

		gSettings.wifiOn = true;
		
		StoreGlobalSettings();
	}
	SetHostname();
	InitializeNextionUi();
	nextionStartCallback();

	if(gSettings.wifiOn)
	{
		if(StartWifi())
		{
			  DebugPrintLn("WiFi connected")
			  DebugPrintLn(GetWifiIp())
		}
	}
	if(gSettings.apOn)
	{
		if(StartAp())
		{
			  DebugPrintLn("AP started")
			  DebugPrintLn(GetSoftApIp())
		}
	}

	configTime(gSettings.timeZoneHour*60*60, 0, "pool.ntp.org", "time.nist.gov");
	time(nullptr);
} 

uint32_t lastimeUpdate{0};
uint32_t timeUpdateInterval{1000};
uint32_t lastInfoUpdate{0};
uint32_t InfoUpdateInterval{5000};
uint8_t prevPage{255};
void loop() 
{
 	// handle Nextion events
	HandleNextionEvents();
	
	uint32 tic=millis();
	if(((prevPage != GetCurrentPage()) && GetCurrentPage()<4)||
	(((lastimeUpdate>tic && (UINT32_MAX-lastimeUpdate + tic)>timeUpdateInterval)|| 
	(lastimeUpdate<tic && (tic-lastimeUpdate)>timeUpdateInterval))&&
	GetCurrentPage()<4))
	{
		lastimeUpdate=tic;
		time_t now = time(nullptr);
		tm* t = localtime(&now);
		if(t->tm_year > 70)
		{
			char timeBuf[6];
			sprintf(timeBuf,"%02i:%02i",t->tm_hour,t->tm_min);
			UpdateTime(timeBuf);
			// update once a minute
			timeUpdateInterval = (60-t->tm_sec+1)*1000;
		}
	}
	if((prevPage!=3 && GetCurrentPage()==3) ||
	(GetCurrentPage()==3 && 
	((lastInfoUpdate>tic && (UINT32_MAX-lastInfoUpdate + tic)>InfoUpdateInterval)|| 
	(lastInfoUpdate<tic && (tic-lastInfoUpdate)>InfoUpdateInterval))))
	{
		lastInfoUpdate = tic;

		String info;
		GetWifiInfo(info);
		UpdateWifiInfo(info);
	}

	prevPage = GetCurrentPage();
}