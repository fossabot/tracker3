//
//  UbxPackets.h
//  horizon-tracker
//
//  Created by Eduardo Carvalho on 10/6/17.
//  Copyright © 2017 Strider. All rights reserved.
//
#include "Arduino.h"

#ifndef __UbxPackets_h
#define __UbxPackets_h
//GPS HEADERS
const unsigned char UBXGPS_HEADER[] = { 0xB5, 0x62 };
const unsigned char NMEAGPS_HEADER[] = { 0x24, 0x47 };
const unsigned char UBX_NAV_PVT[] = { 0x01, 0x07 };
const unsigned char UBX_ACK[] = { 0x05, 0x01 };
const unsigned char UBX_NAK[] = { 0x05, 0x00 };
const unsigned char UBX_ESF_STATUS[] = { 0x10, 0x10 };

//UBX PACKETS
struct UbxNavPvt {
    
    unsigned char   headerClass;
    unsigned char   headerId;
    unsigned short  headerLength;
    unsigned long   iTOW;       // ms     GPS time of week of the navigation epoch. See the description of iTOW for details.
    unsigned short  year;       // y      Year UTC
    unsigned char   month;      // month  Month, range 1..12 UTC
    unsigned char   day;        // d      Day of month, range 1..31 UTC
    unsigned char   hour;       // h      Hour of day, range 0..23 UTC
    unsigned char   min;        // min    Minute of hour, range 0..59 UTC
    unsigned char   sec;        // s      Seconds of minute, range 0..60 UTC
    char            valid;      // -      Validity Flags (see graphic below)
    unsigned long   tAcc;       // ns     Time accuracy estimate UTC
    long            nano;       // ns     Fraction of second, range -1e9..1e9 UTC
    unsigned char   fixType;    // -      GNSSfix Type, range 0..5
    char            flags;      // -      Fix Status Flags (see graphic below)
    unsigned char   flags2;  // -      Reserved
    unsigned char   numSV;      // -      Number of satellites used in Nav Solution
    long            lon;        // deg    Longitude (1e-7)
    long            lat;        // deg    Latitude (1e-7)
    long            height;     // mm     Height above Ellipsoid
    long            hMSL;       // mm     Height above mean sea level
    unsigned long   hAcc;       // mm     Horizontal Accuracy Estimate
    unsigned long   vAcc;       // mm     Vertical Accuracy Estimate
    long            velN;       // mm/s   NED north velocity
    long            velE;       // mm/s   NED east velocity
    long            velD;       // mm/s   NED down velocity
    long            gSpeed;     // mm/s   Ground Speed (2-D)
    long            heading;    // deg    Heading of motion 2-D (1e-5)
    unsigned long   sAcc;       // mm/s   Speed Accuracy Estimate
    unsigned long   headingAcc; // deg    Heading Accuracy Estimate (1e-5)
    unsigned short  pDOP;       // -      Position DOP (0.01)
    unsigned char   reserved1a;  // -      Reserved
    unsigned char   reserved1b;  // -      Reserved
    unsigned char   reserved1c;  // -      Reserved
    unsigned char   reserved1d;  // -      Reserved
    unsigned char   reserved1e;  // -      Reserved
    unsigned char   reserved1f;  // -      Reserved
    long            headVeh;    // -      Heading of vehicle (2-D)
    short           magDec;  // -      Magnetic declination
    unsigned short  magAcc;  // -      Magnetic declination accuracy
    
public:
    void print(){
        Serial.println("----------------UBX-NAV-PVT----------------");
        Serial.print("iTOW = "); Serial.println(iTOW);
        Serial.print("year = "); Serial.println(year);
        Serial.print("month = "); Serial.println(month);
        Serial.print("day = "); Serial.println(day);
        Serial.print("hour = "); Serial.println(hour);
        Serial.print("min = "); Serial.println(min);
        Serial.print("sec = "); Serial.println(sec);
        Serial.print("valid = "); Serial.println(byte(valid),BIN);
        Serial.print("tAcc(ms) = "); Serial.println(tAcc/ 1000000.0, 6);
        Serial.print("nano(ms) = "); Serial.println(nano/ 1000000.0, 6);
        Serial.print("fixType = "); Serial.println(byte(fixType),HEX);
        Serial.print("flags = "); Serial.println(byte(flags),BIN);
        Serial.print("flags2 = "); Serial.println(byte(flags2),BIN);
        Serial.print("numSV = "); Serial.println(numSV);
        Serial.print("lon = "); Serial.println(lon/ 10000000.0, 7);
        Serial.print("lat = "); Serial.println(lat/ 10000000.0, 7);
        Serial.print("height(m) = "); Serial.println(height/ 1000.0, 3);
        Serial.print("hMSL(m) = "); Serial.println(hMSL/ 1000.0, 3);
        Serial.print("hAcc(m) = "); Serial.println(hAcc/ 1000.0, 3);
        Serial.print("vAcc(m) = "); Serial.println(vAcc/ 1000.0, 3);
        Serial.print("velN(km/h) = "); Serial.println(velN* 0.0036, 5);
        Serial.print("velE(km/h) = "); Serial.println(velE* 0.0036, 5);
        Serial.print("velD(km/h) = "); Serial.println(velD* 0.0036, 5);
        Serial.print("gSpeed(km/h) = "); Serial.println(gSpeed* 0.0036, 5);
        Serial.print("heading(deg) = "); Serial.println(heading/ 100000.0, 5);
        Serial.print("sAcc(km/h) = "); Serial.println(sAcc* 0.0036, 5);
        Serial.print("headingAcc = "); Serial.println(headingAcc/ 100000.0, 5);
        Serial.print("pDOP = "); Serial.println(pDOP);
        Serial.print("reserved1 = "); Serial.print(byte(reserved1a),BIN); Serial.print(byte(reserved1b),BIN); Serial.print(byte(reserved1c),BIN); Serial.print(byte(reserved1d),BIN); Serial.print(byte(reserved1e),BIN); Serial.println(byte(reserved1f),BIN);
        Serial.print("headVeh(deg) = "); Serial.println(headVeh/ 100000.0, 5);
        Serial.print("magDec(deg) = "); Serial.println(magDec/ 100.0, 2);
        Serial.print("magAcc(deg) = "); Serial.println(magAcc/ 100.0, 2);
    };
};

struct SensorStatus {
    unsigned  char  sensStatus1;    // -    Sensor status, part 1 (see graphic below)
    unsigned  char  sensStatus2;    // -    Sensor status, part 2 (see graphic below)
    unsigned  char  freq;           // Hz    Observation frequency
    unsigned  char  fault;          // -    Sensor faults (see graphic below)
};

struct UbxEsfStatus {
    unsigned char headerClass;
    unsigned char headerId;
    unsigned short headerLength;
    unsigned long  iTOW;           // ms     GPS time of week of the navigation epoch. See the description of iTOW for details.
    unsigned char  version;        // -      Message version (2 for this version)
    unsigned char  reserved[7];    // -  Reserved
    unsigned char  fusionMode;        // -      Fusion mode
    unsigned char  reserved2[2];       // -      Reserved
    unsigned char  numSens;        // -    Number of sensors (7 for NEO-M8U)
    SensorStatus    sensStatus[7];
public:
    void print(){
        Serial.println("----------------UBX-ESF-STATUS----------------");
        Serial.print("iTOW = "); Serial.println(iTOW);
        Serial.print("version = "); Serial.println(version);
        Serial.print("fusionMode = "); Serial.println(fusionMode);
        Serial.print("numSens = "); Serial.println(numSens);
        for(int i = 0; i < 7; i++){
            Serial.print("Sensor "); Serial.println(i);
            Serial.print("sensStatus1 = "); Serial.println(byte(sensStatus[i].sensStatus1),BIN);
            Serial.print("sensStatus2 = "); Serial.println(byte(sensStatus[i].sensStatus2),BIN);
            Serial.print("freq = "); Serial.println(byte(sensStatus[i].freq));
            Serial.print("fault = "); Serial.println(byte(sensStatus[i].fault),BIN);
        }
    };
};


#endif // def(__UbxPackets_h)
