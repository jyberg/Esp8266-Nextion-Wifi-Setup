/**
 * @file wifi.cpp
 *
 * Esp8266 Wifi / SoftAP setup
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

#include <ESP8266WiFi.h>
#include "globalSettings.h"
#include "wifi.h"

namespace wifi
{

void DisableWifi()
{
    WiFi.softAPdisconnect(true); 
}
void SetHostname()
{
	WiFi.hostname(gSettings.hostname);
}
bool StartWifi()
{
    // Connect to WiFi
    WiFi.begin(gSettings.wlanSsid, gSettings.wlanPwd);

    for(int8_t i{0};  WiFi.status() != WL_CONNECTED && i < 10; ++i)
    {
        delay(200);
    }
    return WiFi.status() == WL_CONNECTED;
}

bool StopWifi()
{
	return WiFi.disconnect();
}

bool StartAp()
{
	return WiFi.softAP(gSettings.apSsid, gSettings.apPwd[0]?gSettings.apPwd:nullptr, gSettings.apChannel, gSettings.apHide, 8);
}

bool StopAp()
{
	return WiFi.softAPdisconnect();
}
IPAddress GetWifiIp()
{
    return WiFi.localIP();
}

IPAddress GetSoftApIp()
{
    return WiFi.softAPIP();
}

void GetWifiInfo(String &info)
{
    info = "Hostname: ";
    info += WiFi.hostname();
    info += "\\r\\r";

    if(!gSettings.apOn)
    {
        info += "Access point Off\\r\\r";
    }
    else
    {
        char tmp[5];
        itoa(WiFi.channel(),tmp,10);
        
        info += "Access point: ";
        info += WiFi.softAPSSID();
        info += "\\rPassword: ";
        info += WiFi.softAPPSK();
        info += "\\rChannel: ";
        info += tmp;
        info += "\\rIP: ";
        info += WiFi.softAPIP().toString();
        info += "\\r\\r";
    }

    if(!gSettings.wifiOn)
    {
        info += "Wifi Off\\r";
    }
    else if(WiFi.status() != WL_CONNECTED)
    {
        info += "Wifi Connection failure\\r";
    }
    else
    {
        info += "Connected Wifi: ";
        info += WiFi.SSID();
        info += "\\rIP: ";
        info += WiFi.localIP().toString();
    }
}

} // namespace
