/**
 * @file wifi.h
 *
 * wifi interface definitions
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
#include <IPAddress.h>

namespace wifi
{
void DisableWifi();
void SetHostname();
bool StartWifi();
bool StopWifi();
bool StartAp();
bool StopAp();
IPAddress GetWifiIp();
IPAddress GetSoftApIp();
void GetWifiInfo(String &info);

}