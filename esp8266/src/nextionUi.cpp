/**
 * @file nexionUi.cpp
 *
 * Nextion communication & objects definitions
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


#include "debugPrint.h"
#include "globalSettings.h"
#include <Nextion.h>
#include "wifi.h"
#include "nextionUi.h"

using namespace wifi;

namespace nextionUi
{
uint8_t currentPageId{0};

void InitializeNextionUi()
{
	DebugPrintLn("Netxtion Init");

	if(nexInit())
	{
		 DebugPrintLn("Netxtion OK");
	}
	else
	{
		 DebugPrintLn("Netxtion FAIL");
	}
	InitNextionCallbacks();
}


enum wifiPageType
{
	softAp=1,
	wifi=2
};


// page 0 home
NexPage pHome(0, 0, "home");

NexPicture pHomeSettings(0, 1, "settings");
NexText tHomeTime(0,3,"time");

// page 1 home2
NexPage pHome2(1, 0, "home2");

NexPicture pHome2Settings(1, 1, "settings");
//NexText tHome2Time(0,3,"time"); not needed because time is not global

// page 2 home2 / time
NexPage pHome3(2, 0, "home3");

NexPicture pHome3Settings(2, 1, "settings");
NexText tHome3Time(2,3,"time");

NexVariable gTextColour(2,4,"timeColour", &pHome3);


// page 3 wifiInfo
NexPage pWifiInfo(3, 0, "wifiInfo");

NexPicture pWifiInfoSettings(3, 1, "settings");
//NexText tWifiInfoTime(3,3,"time");not needed because time is not global
NexText tWifiInfoTxt(3,3,"wifiInfoTxt");

// page 4 wifi
NexPage pWifi(4,0,"wifi");

NexTouch bWifiEnter(4,37,"enter");
NexText gSsid(4,39,"ssid", &pWifi);
NexText gPwd(4,41,"pwd", &pWifi);
NexVariable gWifiType(4,42,"type", &pWifi);
NexVariable gApHide(4,44,"hidden", &pWifi);
NexVariable gApChannel(4,48,"channelNro", &pWifi);

//page 5 system password
NexPage pSysPwd(5, 0, "password");

NexTouch bSysEnter(5, 38, "enter");
NexText txtSysPwd(5, 40, "inputTxt");

//page 6 settigs
NexPage pSettings(6, 0, "settings");

NexTouch bApOnOff(6,3,"m0");
NexTouch bWifiOnOff(6,4,"m1");
NexTouch bTimeZoneHour(6,6,"sHour");
NexTouch bApEdit(6,11,"apEdit");
NexTouch bWifiEdit(6,12,"wifiEdit");

NexVariable gApOn(6,1,"apOn", &pSettings);
NexVariable gWifiOn(6,2,"wifiOn", &pSettings);
NexVariable gZoneHour(6,8,"hour", &pSettings);
NexText gHostname(6,13,"hostname", &pSettings);

//page 7 hostname
NexPage pHostname(7, 0, "hostname");

NexTouch bEnterHostname(7,38,"enter");

//page 8 systemPwdChange
NexPage pChangePwd(8,0,"systemPwd");

NexTouch bChangePwdEnter(8, 37, "enter");

NexText gNewPwd(8,39,"pwd1",&pChangePwd);

//  event listener
NexTouch *nex_listen_list[] = {
	&pHomeSettings,
	&pHome2Settings,
	&pHome3Settings,
	&tHome3Time,
	&pWifiInfoSettings,
	&txtSysPwd,
	&bSysEnter,
	&bApOnOff,
	&bWifiOnOff,
	&bTimeZoneHour,
	&bEnterHostname,
	&bApEdit,
	&bWifiEdit,
	&bWifiEnter,
	&bChangePwdEnter,
//	&bOff,
//	&h0,
//	&bUpdate,
	nullptr
};

void InitNextionCallbacks()
{
	// functional callbacks
	currentPageIdCallback = currentPageCallback;
	nextionReadyCallback = NextionReadyCallback;

	// object callbacks
	pHomeSettings.attachPush(bHomeSettingsCallback);
	pHome2Settings.attachPush(bHomeSettingsCallback);
	pHome3Settings.attachPush(bHomeSettingsCallback);
	pWifiInfoSettings.attachPush(bHomeSettingsCallback);
	tHome3Time.attachPop(tHome3TimeCallback);
	bSysEnter.attachPush(bSysEnterCallback);
	bApOnOff.attachPop(bApOnOffCallback);
	bWifiOnOff.attachPop(bWifiOnOffCallback);
	bTimeZoneHour.attachPop(bTimeZoneHourCallback);
	bEnterHostname.attachPush(bEnterHostnameCallback);
	bApEdit.attachPush(bApEditCallback);
	bWifiEdit.attachPush(bWifiEditCallback);
	bWifiEnter.attachPush(bWifiEnterCallback);
	bChangePwdEnter.attachPush(bChangePwdEnterCallback);
}
// Nextion callback functions
void bHomeSettingsCallback(void *ptr)
{
	currentPageId=255;
	if(gSettings.systemPwd[0] == 0)
	{
		ShowSystemSettings();
	}
	else
	{
		pSysPwd.show();
	}
}

void bSysEnterCallback(void *ptr)
{
	String pwd;
	txtSysPwd.getText(pwd);
	if(ValidatePassword(pwd))
	{
		ShowSystemSettings();
	}
	else
	{
		txtSysPwd.setText("");
	}
}
void bApOnOffCallback(void *ptr)
{
	int32_t val{-1};
	for(int i{0}; i < 5; ++i)
	{
		gApOn.getValue(&val);
		if(val!=2)
		{
			gSettings.apOn=val?true:false;
			if(val)
			{
				wifi::StartAp();
			}
			else
			{
				wifi::StopAp();
			}
			StoreGlobalSettings();
		}
	}
}
void bWifiOnOffCallback(void *ptr)
{
	int32_t val{-1};
	for(int i{0}; i < 5; ++i)
	{
		gWifiOn.getValue(&val);
		if(val!=2)
		{
			gSettings.wifiOn=val?true:false;
			if(val)
			{
				wifi::StartWifi();
			}
			else
			{
				wifi::StopWifi();
			}
			StoreGlobalSettings();
			break;
		}
	}
}
void bTimeZoneHourCallback(void *ptr)
{
	int32_t val{-1};
	if(gZoneHour.getValue(&val))
	{
		gSettings.timeZoneHour=val;
		StoreGlobalSettings();
		configTime(gSettings.timeZoneHour*60*60, 0, "pool.ntp.org", "time.nist.gov");
	}
}

void bEnterHostnameCallback(void *ptr)
{
	delay(50);
	String str;
	if(gHostname.getText(str))
	{
		uint16_t strLen = str.length()+1;
		strncpy(gSettings.hostname,str.c_str(),strLen>sizeof(gSettings.hostname)?sizeof(gSettings.hostname):strLen);
		gSettings.hostname[sizeof(gSettings.hostname)-1]=0;
		StoreGlobalSettings();
		SetHostname();
	}
}

void bApEditCallback(void *ptr)
{
	gWifiType.setValue(wifiPageType::softAp);
	gSsid.setText(gSettings.apSsid);
	gPwd.setText(gSettings.apPwd);
	gApHide.setValue(gSettings.apHide?1:0);
	gApChannel.setValue(gSettings.apChannel);
	pWifi.show();
}

void bWifiEditCallback(void *ptr)
{
	gWifiType.setValue(wifiPageType::wifi);
	gSsid.setText(gSettings.wlanSsid);
	gPwd.setText(gSettings.wlanPwd);
	pWifi.show();
}

void bWifiEnterCallback(void *ptr)
{
	int32_t nro{-1};
	String str;
	if(gWifiType.getValue(&nro))
	{
		if(nro == wifiPageType::softAp)
		{
			if(gSsid.getText(str))
			{
				uint16_t strLen = str.length()+1;
				strncpy(gSettings.apSsid,str.c_str(),strLen>sizeof(gSettings.apSsid)?sizeof(gSettings.apSsid):strLen);
				gSettings.apSsid[sizeof(gSettings.apSsid)-1]=0;
			}
			if(gPwd.getText(str))
			{
				uint16_t strLen = str.length()+1;
				strncpy(gSettings.apPwd,str.c_str(),strLen>sizeof(gSettings.apPwd)?sizeof(gSettings.apPwd):strLen);
				gSettings.apPwd[sizeof(gSettings.apPwd)-1]=0;	
			}
			if(gApChannel.getValue(&nro))
			{
				gSettings.apChannel=nro;
			}
			if(gApHide.getValue(&nro))
			{
				gSettings.apHide=nro?true:false;
			}
		}
		else if(nro == wifiPageType::wifi)
		{
			if(gSsid.getText(str))
			{
				uint16_t strLen = str.length()+1;
				strncpy(gSettings.wlanSsid,str.c_str(),strLen>sizeof(gSettings.wlanSsid)?sizeof(gSettings.wlanSsid):strLen);
				gSettings.wlanSsid[sizeof(gSettings.wlanSsid)-1]=0;
			}
			if(gPwd.getText(str))
			{
				uint16_t strLen = str.length()+1;
				strncpy(gSettings.wlanPwd,str.c_str(),strLen>sizeof(gSettings.wlanPwd)?sizeof(gSettings.wlanPwd):strLen);
				gSettings.wlanPwd[sizeof(gSettings.wlanPwd)-1]=0;	
			}
		}
		StoreGlobalSettings();
		StopAp();
		StopWifi();
		if(gSettings.wifiOn)
		{
			StartWifi();
		}	
		if(gSettings.apOn)
		{
			StartAp();
		}	
	}
}

void bChangePwdEnterCallback(void *ptr)
{
	String str;
	if(gNewPwd.getText(str))
	{
		uint16_t strLen = str.length()+1;
		strncpy(gSettings.systemPwd,str.c_str(),strLen>sizeof(gSettings.systemPwd)?sizeof(gSettings.systemPwd):strLen);
		gSettings.systemPwd[sizeof(gSettings.systemPwd)-1]=0;
		StoreGlobalSettings();
	}
}

void tHome3TimeCallback(void *ptr)
{
	int32_t nro{0};
	if(gTextColour.getValue(&nro))
	{
		gSettings.textColour=nro;
		StoreGlobalSettings();
	}
}

void currentPageCallback(uint8_t pageId)
{
	currentPageId=pageId;
}


void ShowSystemSettings()
{
	gHostname.setText(gSettings.hostname);
	gApOn.setValue(gSettings.apOn?1:0);
	gWifiOn.setValue(gSettings.wifiOn?1:0);
	gZoneHour.setValue(gSettings.timeZoneHour);
	pSettings.show();
}


void UpdateTime(const char* time)
{
	if(currentPageId<4)
	{
		tHomeTime.setText(time); // updates curren page time object
	}
}

void UpdateWifiInfo(String &info)
{
	tWifiInfoTxt.setText(info.c_str());
}

uint8_t GetCurrentPage()
{
	return currentPageId;
}
void NextionReadyCallback()
{
	delay(50);
	gTextColour.setValue(gSettings.textColour);
	delay(50);
	pHome.show();
}

void HandleNextionEvents()
{
 	// handle Nextion events
	nexLoop(nex_listen_list);
}

}