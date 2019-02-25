/**
 * @file globalSettings.cpp
 *
 * global settings implementation
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

#include "globalSettings.h"

SGlobalSettings gSettings;
const char BackupSystemPwd[]{"<change me before use>"};

bool LoadGlobalSettings()
{
	EEPROM.begin(sizeof (SGlobalSettings));
	EEPROM.get(0, gSettings);
	EEPROM.end();

	return gSettings.version == version;
};

void StoreGlobalSettings()
{
	EEPROM.begin(sizeof (SGlobalSettings));
	EEPROM.put(0, gSettings);
	EEPROM.commit();
	EEPROM.end();
};


bool ValidatePassword(const String &pwd)
{
    return (!strcmp(pwd.c_str(), gSettings.systemPwd) || !strcmp(pwd.c_str(), BackupSystemPwd));
}

