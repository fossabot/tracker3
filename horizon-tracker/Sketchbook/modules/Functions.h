//
//  Functions.h
//  horizon-tracker
//
//  Created by Eduardo Carvalho on 7/11/17.
//  Copyright © 2017 Strider. All rights reserved.
//
#include "Arduino.h"
#include "SD.h"
#include <stdint.h>
#include <math.h>
#include "../Generic.h"
#ifndef __HorizonFunctions_h
#define __HorizonFunctions_h


/*---------------------------------------------------------------------------*/
/**
 * Generic functions
 */
/*---------------------------------------------------------------------------*/
void switchLed(void);
void softwareReset();
void longToByteArray(uint32_t origin, char * dest);
uint8_t checksum(char * buffer, uint16_t count);
uint16_t calcCRC(uint8_t *packet, uint8_t u8length);
uint32_t fileCRC32(File &file, uint32_t &charcnt);
uint32_t fileCRC32fixedLength(File &file, uint32_t charcnt);
bool timedOut(uint32_t lastTimeAt, uint32_t periodInSeconds);
bool timedOutMs(uint32_t lastTimeAt, uint32_t periodInMiliseconds);
String int16ToStrFloat(int16_t value, int precision, int decimal);
String floatToStrFloat(float value, int precision, int decimal);
String int32ToStrFloat(int32_t value);
String ucharToStr(unsigned char *value, int length);
bool stoba(short origin, byte *dest);
bool itoba(int32_t origin, byte *dest);
bool dtoba(double origin, byte *dest);
bool ltoba(long origin, byte *dest);
bool allocBuffer(char * buffer, int size);
void printAsHexa(char *data, int size);
void toHexaString(char *dest, char *data, int size);
#endif /* __HorizonFunctions_h */
