/**
 * @file nextionUi.h
 *
 * nextionUi definitions
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

namespace nextionUi
{
// interface functions
void InitializeNextionUi();
void UpdateTime(const char *time);
void UpdateWifiInfo(String &info);
uint8_t GetCurrentPage();

void InitNextionCallbacks();
// Nextion callback functions
void bHomeSettingsCallback(void *ptr);
void bSysEnterCallback(void *ptr);
void bApOnOffCallback(void *ptr);
void bWifiOnOffCallback(void *ptr);
void bTimeZoneHourCallback(void *ptr);
void bEnterHostnameCallback(void *ptr);
void bApEditCallback(void *ptr);
void bWifiEditCallback(void *ptr);
void bWifiEnterCallback(void *ptr);
void bChangePwdEnterCallback(void *ptr);
void tHome3TimeCallback(void *ptr);

void currentPageCallback(uint8_t pageId);
void nextionStartCallback();

// helper functions
void ShowSystemSettings();

// events handler
void HandleNextionEvents();

}