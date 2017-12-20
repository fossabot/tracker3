//
//  Commands.h
//  horizon-tracker
//
//  Created by Eduardo Carvalho on 7/18/17.
//  Copyright © 2017 Strider. All rights reserved.
//
#include "Arduino.h"
#include "../Generic.h"
#ifndef HorizonCommands_h
#define HorizonCommands_h

#define ORB_HEADER_SIZE 14
#define FW_OK 0
#define FW_CORRUPTED 1
#define MISSING_SD 2
#define FW_OUTOFDATE 3
#define FW_FILENAME "FIRMWARE.BIN"

struct OrbBluetoothHeader {
    //HEADER
    char        packetRemoteAddress[8];
    uint8_t     packetSourceEndpoint;
    uint8_t     packetDestinationEndpoint;
    uint16_t    packetClusterId;
    uint16_t    packetProfileId;
    char        packetUUID[16];
    
public:
    char        *getPacketRemoteAddress()    { return packetRemoteAddress; }
    uint8_t     getPacketSourceEndpoint() { return packetSourceEndpoint; }
    uint8_t     getPacketDestinationEndpoint() { return packetDestinationEndpoint; }
    uint16_t    getPacketClusterId() { return packetClusterId; }
    uint16_t    getPacketProfileId() { return packetProfileId; }
    char        *getPacketUUID() { return packetUUID; }
    
    bool    parse(char *data);
    void toArray(char *rawPacket);
    
};

void printLcdData(char *deviceSerial, char *remoteDeviceSerial, char *bluetoothSerial, uint8_t signalPower, bool bluetoothConnected, bool sdCardStatus);
void updateLcdData(uint8_t signalPower, bool bluetoothConnected);
int verifyFirmware();

bool analyseXbeePack();

bool saveLog(char *logText, unsigned long fileNumber);
void copyFileQueue(char *logFileName);
#endif /* __HorizonCommands_h */
