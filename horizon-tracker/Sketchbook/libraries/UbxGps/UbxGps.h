/**
 * UBX GPS Library
 * Created by Danila Loginov, July 23, 2016
 * https://github.com/1oginov/UBX-GPS-Library
 */

#ifndef UBXGPS_H_
#define UBXGPS_H_

#include "Arduino.h"

const unsigned char UBXGPS_HEADER[] = { 0xB5, 0x62 };

class UbxGps {
public:
    void begin(long);
    boolean ready();

protected:
    UbxGps(HardwareSerial&);
    void setLength(unsigned char);

private:
    int available();
    byte read();
    void calculateChecksum();

    // Class properties
    HardwareSerial& serial;
    unsigned char offsetClassProperties = 8;
    unsigned char offsetHeaders = 4;
    unsigned char size;
    unsigned char carriagePosition;
    unsigned char chksum[2];

    // Headers (common)
    unsigned char headerClass;
    unsigned char headerId;
    unsigned short headerLength;
};

struct DataSample {
    unsigned  long  iTOW;       // ms     GPS time of week of the navigation epoch. See the description of iTOW for details.
    unsigned  short year;       // y      Year UTC
    unsigned  char  month;      // month  Month, range 1..12 UTC
    unsigned  char  day;        // d      Day of month, range 1..31 UTC
    unsigned  char  hour;       // h      Hour of day, range 0..23 UTC
    unsigned  char  min;        // min    Minute of hour, range 0..59 UTC
    unsigned  char  sec;        // s      Seconds of minute, range 0..60 UTC
    char            valid;      // -      Validity Flags (see graphic below)
    unsigned  long  tAcc;       // ns     Time accuracy estimate UTC
    long            nano;       // ns     Fraction of second, range -1e9..1e9 UTC
    unsigned  char  fixType;    // -      GNSSfix Type, range 0..5
    char            flags;      // -      Fix Status Flags (see graphic below)
    unsigned  char  flags2;  // -      Reserved
    unsigned  char  numSV;      // -      Number of satellites used in Nav Solution
    long            lon;        // deg    Longitude (1e-7)
    long            lat;        // deg    Latitude (1e-7)
    long            height;     // mm     Height above Ellipsoid
    long            hMSL;       // mm     Height above mean sea level
    unsigned  long  hAcc;       // mm     Horizontal Accuracy Estimate
    unsigned  long  vAcc;       // mm     Vertical Accuracy Estimate
    long            velN;       // mm/s   NED north velocity
    long            velE;       // mm/s   NED east velocity
    long            velD;       // mm/s   NED down velocity
    long            gSpeed;     // mm/s   Ground Speed (2-D)
    long            heading;    // deg    Heading of motion 2-D (1e-5)
    unsigned  long  sAcc;       // mm/s   Speed Accuracy Estimate
    unsigned  long  headingAcc; // deg    Heading Accuracy Estimate (1e-5)
    unsigned  short pDOP;       // -      Position DOP (0.01)
    unsigned  char  reserved1a;  // -      Reserved
    unsigned  char  reserved1b;  // -      Reserved
    unsigned  char  reserved1c;  // -      Reserved
    unsigned  char  reserved1d;  // -      Reserved
    unsigned  char  reserved1e;  // -      Reserved
    unsigned  char  reserved1f;  // -      Reserved
    long            headVeh;    // -      Heading of vehicle (2-D)
    unsigned  char  reserved2a;  // -      Reserved
    unsigned  char  reserved2b;  // -      Reserved
    unsigned  char  reserved2c;  // -      Reserved
    unsigned  char  reserved2d;  // -      Reservedpublic:
};
#endif
