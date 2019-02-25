/**
 * @file debugPrint.h
 *
 * DebugPrint definitions
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

#include <HardwareSerial.h>

#ifdef DEBUG

#define DebugPrintLn(msg) Serial.println(msg);

#else

#define DebugPrintLn(msg) {}

#endif

/*
inline void DebugPrintLn(const char* msg)
{
//if constexpr(DEBUG)
#ifdef DEBUG
{
    Serial.println(msg); 
}
#endif
}

inline void DebugPrintLn(const Printable &x)
{
//if constexpr(DEBUG)
#ifdef DEBUG
{
    Serial.println(x);
}
#endif
}
*/