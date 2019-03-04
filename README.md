# Esp8266-Nextion-Wifi-Setup
Esp8266 wifi setup via Nextion UI

Esp8266 Wifi/Soft AP setup via Nextion UI.
After Wifi is setup and connected Esp8266 updates current time in home pages, and Wifi details shown in home page 3.
Project uses NodeMcu Harwareserial because Softwareserial not work all ways in NodeMcu/Esp8266 - Nextion communication (see tips/hints from my Enhanced Nextion library repository).

NodeMcu/Esp8266 project done using platform IO. Also corrected / modified Netion library used inluded in library folder (https://github.com/jyberg/Enhanced-Nextion-Library)

Used Nextion fonts are created using font editor thanks to: https://github.com/hagronnestad/nextion-font-editor

I hope that this also helps to learn Esp8266-Nextion communication and tips & trics. Commens well come...

Remember to change master backup password in globalsettings.cpp

![Screenshot](/Nextion/pictures/SystemSettings.bmp)

