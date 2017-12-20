//
//  SDutils.h
//  horizon-tracker
//
//  Created by Eduardo Carvalho on 7/12/17.
//  Copyright © 2017 Strider. All rights reserved.
//
#include "Arduino.h"
#include "../Generic.h"
#ifndef __HorizonSDutils_h
#define __HorizonSDutils_h

namespace nmSdCard {
    bool setupSDCard(void);
    void printDirectory(HardwareSerial &serialPort, char * path, int numTabs) ;
    void printDirectoryXbee(char * path, int numTabs);
    bool isSDPresent(void);
    bool dumpFile(HardwareSerial &serialPort, char * fileName, bool delayBetweenLines);
    bool writeSDline(char * logFileName, char * text);
    bool writeSDline(String logFileName, char * text);
    bool overwriteSDline(char *logFileName, char *text);
    bool readLine(const char * logFileName, char *myLine);
    bool readNextLog(char *logFileName, char *myLine);
    bool exists(const char *fileName);
    bool remove(const char *fileName);
    bool isEmpty(const char *fileName);
    bool mkDir(const char *path);
    bool rmDir(const char *path);
    bool isOk();
    uint32_t fileSize(const char *path);
}

#endif // def(__HorizonGPS_h)
