/**
 * @file globalSettings.h
 *
 * global settings definitions
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

#pragma once
#include <WString.h>
#include <stdint.h>
#include <EEPROM.h>

const int version{110}; // SGlobalSettings struct version
struct SGlobalSettings
{
	int version;
	char systemPwd[64];
	char hostname[32];
	char wlanSsid[32];
	char wlanPwd[64];
	char apSsid[32];
	char apPwd[64];
	uint8_t apChannel;
	bool apHide;
	bool wifiOn;
	bool apOn;
	int8_t timeZoneHour;
	uint16_t textColour;
};
extern SGlobalSettings gSettings;

bool LoadGlobalSettings();
void StoreGlobalSettings();

bool ValidatePassword(const String &pwd);
