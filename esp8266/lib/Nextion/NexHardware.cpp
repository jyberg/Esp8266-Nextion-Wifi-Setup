/**
 * @file NexHardware.cpp
 *
 * The implementation of base API for using Nextion. 
 *
 * @author  Wu Pengfei (email:<pengfei.wu@itead.cc>)
 * @date    2015/8/11
 * @author Jyrki Berg 2/17/2019 (https://github.com/jyberg)
 * 
 * @copyright 
 * Copyright (C) 2014-2015 ITEAD Intelligent Systems Co., Ltd. \n
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 */
#include "NexHardware.h"


#ifdef NEX_SOFTWARE_SERIAL
#include <SoftwareSerial.h>
SoftwareSerial nexSerial(NEX_RX,NEX_TX);
#endif

#define NEX_RET_EVENT_NEXTION_STARTUP       (0x00)
#define NEX_RET_EVENT_TOUCH_HEAD            (0x65)
#define NEX_RET_CURRENT_PAGE_ID_HEAD        (0x66)
#define NEX_RET_EVENT_POSITION_HEAD         (0x67)
#define NEX_RET_EVENT_SLEEP_POSITION_HEAD   (0x68)
#define NEX_RET_STRING_HEAD                 (0x70)
#define NEX_RET_NUMBER_HEAD                 (0x71)
#define NEX_RET_AUTOMATIC_SLEEP             (0x86)
#define NEX_RET_AUTOMATIC_WAKE_UP           (0x87)
#define NEX_RET_EVENT_NEXTION_READY         (0x88)
#define NEX_RET_START_SD_UPGRADE            (0x89)
#define Nex_RET_TRANSPARENT_DATA_FINISHED   (0xFD)
#define Nex_RET_TRANSPARENT_DATA_READY      (0xFE)

#define NEX_RET_INVALID_CMD             (0x00)
#define NEX_RET_CMD_FINISHED_OK         (0x01)
#define NEX_RET_INVALID_COMPONENT_ID    (0x02)
#define NEX_RET_INVALID_PAGE_ID         (0x03)
#define NEX_RET_INVALID_PICTURE_ID      (0x04)
#define NEX_RET_INVALID_FONT_ID         (0x05)
#define NEX_RET_INVALID_FILE_OPERATION  (0x06)
#define NEX_RET_INVALID_CRC             (0x09)
#define NEX_RET_INVALID_BAUD            (0x11)
#define NEX_RET_INVALID_WAVEFORM_ID_OR_CHANNEL_NRO  (0x12)
#define NEX_RET_INVALID_VARIABLE_OR_ATTRIBUTE       (0x1A)
#define NEX_RET_INVALID_VARIABLE_OPERATION          (0x1B)
#define NEX_RET_ASSIGNMENT_FAILED_TO_ASSIGN         (0x1C)
#define NEX_RET_EEPROM_OPERATION_FAILED             (0x1D)
#define NEX_RET_INVALID_QUANTITY_OF_PARAMETERS      (0x1E)
#define NEX_RET_IO_OPERATION_FAILED                 (0x1F)
#define NEX_RET_ESCAPE_CHARACTER_INVALID            (0x20)
#define NEX_RET_VARIABLE_NAME_TOO_LONG              (0x23)
#define NEX_RET_SERIAL_BUFFER_OVERFLOW              (0x24)


void (*nextionStartupCallback)() =nullptr;
void (*currentPageIdCallback)(uint8_t) =nullptr;
void (*touchCoordinateCallback)(uint16_t,uint16_t,uint8_t)=nullptr;
void(*touchEventInSleepModeCallback)(uint16_t,uint16_t,uint8_t) =nullptr;
void (*automaticSleepCallback)() =nullptr;
void (*automaticWakeUpCallback)() =nullptr;
void (*nextionReadyCallback)() =nullptr;
void (*startSdUpgradeCallback)() =nullptr;

/*
 * Receive unt32_t data. 
 * 
 * @param number - save uint32_t data. 
 * @param timeout - set timeout time. 
 *
 * @retval true - success. 
 * @retval false - failed.
 *
 */
bool recvRetNumber(uint32_t *number, uint32_t timeout)
{
    bool ret = false;
    uint8_t temp[8] = {0};

    if (!number)
    {
        goto __return;
    }
    
    nexSerial.setTimeout(timeout);
    if (sizeof(temp) != nexSerial.readBytes((char *)temp, sizeof(temp)))
    {
        goto __return;
    }

    if (temp[0] == NEX_RET_NUMBER_HEAD
        && temp[5] == 0xFF
        && temp[6] == 0xFF
        && temp[7] == 0xFF
        )
    {
        *number = ((uint32_t)temp[4] << 24) | ((uint32_t)temp[3] << 16) | ((uint32_t)temp[2] << 8) | (temp[1]);
        ret = true;
    }

__return:

    if (ret) 
    {
        dbSerialPrint("recvRetNumber :");
        dbSerialPrintln(*number);
    }
    else
    {
        dbSerialPrintln("recvRetNumber err");
    }
    
    return ret;
}

/*
 * Receive int32_t data. 
 * 
 * @param number - save int32_t data. 
 * @param timeout - set timeout time. 
 *
 * @retval true - success. 
 * @retval false - failed.
 *
 */
bool recvRetNumber(int32_t *number, uint32_t timeout)
{
    bool ret = false;
    uint8_t temp[8] = {0};

    if (!number)
    {
        goto __return;
    }
    
    nexSerial.setTimeout(timeout);
    if (sizeof(temp) != nexSerial.readBytes((char *)temp, sizeof(temp)))
    {
        goto __return;
    }

    if (temp[0] == NEX_RET_NUMBER_HEAD
        && temp[5] == 0xFF
        && temp[6] == 0xFF
        && temp[7] == 0xFF
        )
    {
        *number = ((int32_t)temp[4] << 24) | ((int32_t)temp[3] << 16) | ((int32_t)temp[2] << 8) | (temp[1]);
        ret = true;
    }

__return:

    if (ret) 
    {
        dbSerialPrint("recvRetNumber :");
        dbSerialPrintln(*number);
    }
    else
    {
        dbSerialPrintln("recvRetNumber err");
    }
    
    return ret;
}

/*
 * Receive string data. 
 * 
 * @param str - save string data. 
 * @param timeout - set timeout time. 
 *
 * @retval true - success. 
 * @retval false - failed.
 *
 */
bool recvRetString(String &str, uint32_t timeout)
{
    str = "";
    bool ret{false};
    bool str_start_flag = false;
    uint8_t cnt_0xff = 0;
    uint8_t c = 0;
    long start;

    start = millis();
    while (millis() - start <= timeout && ret == false)
    {
        while (nexSerial.available())
        {
            c = nexSerial.read();
            if (str_start_flag)
            {
                if (0xFF == c)
                {
                    cnt_0xff++;                    
                    if (cnt_0xff >= 3)
                    {
                        ret = true;
                        break;
                    }
                }
                else
                {
                    str += (char)c;
                }
            }
            else if (NEX_RET_STRING_HEAD == c)
            {
                str_start_flag = true;
            }
        }
    }

    dbSerialPrint("recvRetString[");
    dbSerialPrint(str.length());
    dbSerialPrint(",");
    dbSerialPrint(str);
    dbSerialPrintln("]");

    return ret;
}

/*
 * Receive string data. 
 * 
 * @param buffer - save string data. 
 * @param len - in buffer len / out saved string len excluding null char. 
 * @param timeout - set timeout time. 
 *
 * @retval true - success. 
 * @retval false - failed.  
 *
 */
bool recvRetString(char *buffer, uint16_t &len, uint32_t timeout)
{
    String temp;
    bool ret = recvRetString(temp,timeout);

    if(ret && len)
    {
        len=temp.length()>len?len:temp.length();
        strncpy(buffer,temp.c_str(), len);
    }
    return ret;
}

/*
 * Send command to Nextion.
 *
 * @param cmd - the string of command.
 */
void sendCommand(const char* cmd)
{
    // empty in buffer for clean responce
    while (nexSerial.available())
    {
        nexSerial.read();
    }
    
    nexSerial.print(cmd);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
    nexSerial.write(0xFF);
}

#ifdef STD_SUPPORT
void sendRawData(const std::vector<uint8_t> &data)
{
    nexSerial.write(data.data(),data.size());
}
#endif

void sendRawData(const uint8_t *buf, uint16_t len)
{
    nexSerial.write(buf, len);
}

void sendRawByte(const uint8_t byte)
{
    nexSerial.write(&byte, 1);
}

bool recvCommand(const uint8_t command, uint32_t timeout)
{
    bool ret = false;
    uint8_t temp[4] = {0};
    
    nexSerial.setTimeout(timeout);
    if (sizeof(temp) != nexSerial.readBytes((char *)temp, sizeof(temp)))
    {
        dbSerialPrintln("recv command timeout");
        ret = false;
    }
    else
    {
        if (temp[0] == command
            && temp[1] == 0xFF
            && temp[2] == 0xFF
            && temp[3] == 0xFF
            )
        {
            ret = true;
        }
        else
        {
            dbSerialPrint("recv command err value: ");
            dbSerialPrintln(temp[0]);   
        }
    }
    return ret;
}

bool recvRetCommandFinished(uint32_t timeout)
{
    bool ret = recvCommand(NEX_RET_CMD_FINISHED_OK, timeout);
    if (ret) 
    {
        dbSerialPrintln("recvRetCommandFinished ok");
    }
    else
    {
        dbSerialPrintln("recvRetCommandFinished err");
    }
    return ret;
}

bool RecvTransparendDataModeReady(uint32_t timeout)
{
    dbSerialPrintln("RecvTransparendDataModeReady requested");
    bool ret = recvCommand(Nex_RET_TRANSPARENT_DATA_READY, timeout);
    if (ret) 
    {
        dbSerialPrintln("RecvTransparendDataModeReady ok");
    }
    else
    {
        dbSerialPrintln("RecvTransparendDataModeReady err");
    }
    return ret;
}

bool RecvTransparendDataModeFinished(uint32_t timeout)
{
    bool ret = recvCommand(Nex_RET_TRANSPARENT_DATA_FINISHED, timeout);
    if (ret) 
    {
        dbSerialPrintln("RecvTransparendDataModeFinished ok");
    }
    else
    {
        dbSerialPrintln("RecvTransparendDataModeFinished err");
    }
    return ret;
}


bool nexInit(const uint32_t baud)
{
    bool ret1 = false;
    bool ret2 = false;

    nexSerial.begin(9600); // default baud, it is recommended that do not change defaul baud on Nextion, because it can forgot it on re-start
    if(baud!=9600)
    {
        char cmd[14];
        sprintf(cmd,"baud=%i",baud);
        sendCommand(cmd);
        delay(100);
        nexSerial.begin(baud);
    }

    sendCommand("bkcmd=3");
    ret1 = recvRetCommandFinished();
    sendCommand("page 0");
    ret2 = recvRetCommandFinished();
    return ret2;
}

void nexLoop(NexTouch *nex_listen_list[])
{
    static uint8_t __buffer[10];
    
    while (nexSerial.available())
    {
        __buffer[0] = nexSerial.read();
        nexSerial.setTimeout(200);
        switch(__buffer[0])
        {
            case NEX_RET_EVENT_NEXTION_STARTUP:
            {
                if(5==nexSerial.readBytes(&__buffer[1],5))
                {
                    if (0x00 == __buffer[1] && 0x00 == __buffer[2] && 0xFF == __buffer[3] && 0xFF == __buffer[4] && 0xFF == __buffer[5])
                    {
                        if(nextionStartupCallback!=nullptr)
                        {
                            nextionStartupCallback();
                        }
                    }
                }
                break;
            }
            case NEX_RET_EVENT_TOUCH_HEAD:
            {
                if(6==nexSerial.readBytes(&__buffer[1],6))
                {
                    if (0xFF == __buffer[4] && 0xFF == __buffer[5] && 0xFF == __buffer[6])
                    {
                        NexTouch::iterate(nex_listen_list, __buffer[1], __buffer[2], __buffer[3]);
                    }
                }
                break;
            }
            case NEX_RET_CURRENT_PAGE_ID_HEAD:
            {
                if(4==nexSerial.readBytes(&__buffer[1],4))
                {
                    if (0xFF == __buffer[2] && 0xFF == __buffer[3] && 0xFF == __buffer[4])
                    {
                        if(currentPageIdCallback!=nullptr)
                        {
                            currentPageIdCallback(__buffer[1]);
                        }
                    }
                }
                break;
            }
            case NEX_RET_EVENT_POSITION_HEAD:
            case NEX_RET_EVENT_SLEEP_POSITION_HEAD:
            {
                if(8==nexSerial.readBytes(&__buffer[1],8))
                {                  
                    if (0xFF == __buffer[6] && 0xFF == __buffer[7] && 0xFF == __buffer[8])
                    {
                        if(__buffer[0] == NEX_RET_EVENT_POSITION_HEAD && touchCoordinateCallback!=nullptr)
                        {
                                
                            touchCoordinateCallback(((int16_t)__buffer[2] << 8) | (__buffer[1]), ((int16_t)__buffer[4] << 8) | (__buffer[3]),__buffer[5]);
                        }
                        else if(__buffer[0] == NEX_RET_EVENT_SLEEP_POSITION_HEAD && touchCoordinateCallback!=nullptr)
                        {
                                
                            touchEventInSleepModeCallback(((int16_t)__buffer[2] << 8) | (__buffer[1]), ((int16_t)__buffer[4] << 8) | (__buffer[3]),__buffer[5]);
                        }
                    }
                }
                break;
            }
            case NEX_RET_AUTOMATIC_SLEEP:
            case NEX_RET_AUTOMATIC_WAKE_UP:
            {
                if(3==nexSerial.readBytes(&__buffer[1],3))
                {
                    if (0xFF == __buffer[1] && 0xFF == __buffer[2] && 0xFF == __buffer[3])
                    {
                        if(__buffer[0]==NEX_RET_AUTOMATIC_SLEEP && automaticSleepCallback!=nullptr)
                        {
                            automaticSleepCallback();
                        }
                        else if(__buffer[0]==NEX_RET_AUTOMATIC_WAKE_UP && automaticWakeUpCallback!=nullptr)
                        {
                            automaticWakeUpCallback();
                        }
                    }
                }
                break;
            }
            case NEX_RET_EVENT_NEXTION_READY:
            {
                if(nextionReadyCallback!=nullptr)
                {
                    nextionReadyCallback();
                }
                break;
            }
            case NEX_RET_START_SD_UPGRADE:
            {
                if(startSdUpgradeCallback!=nullptr)
                {
                    startSdUpgradeCallback();
                }
                break;
            }
            default:
            {
                // unnoun data clean buffer.
                while (nexSerial.available())
                {
                    nexSerial.read();
                }
                break;              
            }
        };      
    }
}
