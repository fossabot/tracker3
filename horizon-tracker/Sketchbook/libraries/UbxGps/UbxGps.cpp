/**
 * UBX GPS Library
 * Created by Danila Loginov, July 23, 2016
 * https://github.com/1oginov/UBX-GPS-Library
 */

#include "UbxGps.h"
#include "Arduino.h"
UbxGps::UbxGps(HardwareSerial& serial) : serial(serial) {
    this->carriagePosition = 0;
}

void UbxGps::setLength(unsigned char length) {
    this->size = length + this->offsetHeaders;
}

void UbxGps::begin(long speed) {
    return this->serial.begin(speed);
}

int UbxGps::available() {
    return this->serial.available();
}

byte UbxGps::read() {
    return this->serial.read();
}

void UbxGps::calculateChecksum() {
    memset(this->chksum, 0, 2);
    for (int i = 0; i < this->size; i++) {
        this->chksum[0] += ((unsigned char*)(this))[i + this->offsetClassProperties];
        this->chksum[1] += this->chksum[0];
    }
}

boolean UbxGps::ready() {
    unsigned char p = this->carriagePosition;

    DataSample dataSample;
    while (this->available()) {
        byte c = this->read();

        // Carriage is at first or second sync byte, should be equals
        if (p < 2) {
            if (c == UBXGPS_HEADER[p]) {
                p++;
            }
            // Reset if not
            else {
                p = 0;
                Serial.println("HEADER ERROR");
            }
        }

        // After successful sync with header
        else {

            // Put byte read to particular address of this object which depends on carriage position
            if (p < (this->size + 2)) {
                ((unsigned char*)(this))[p - 2 + this->offsetClassProperties] = c;
                if(p>=6){
                    ((unsigned char*)(&dataSample))[p-6] = c;
                }
            }

            // Move the carriage forward
            p++;

            // Carriage is at the first checksum byte, we can calculate our checksum, but not compare because this byte is not read
            if (p == (this->size + 2)) {
                this->calculateChecksum();
            }

            // Carriage is at the second checksum byte, but only the first byte of checksum read, check if it equals to ours
            else if (p == (this->size + 3)) {
                // Reset if not
                if (c != this->chksum[0]) {
                    p = 0;
                    
                    Serial.println("CHECKSUM ERROR");
                }
            }

            // Carriage is after the second checksum byte, which has been read, check if it equals to ours
            else if (p == (this->size + 4)) {
                // Reset the carriage
                p = 0;
                // The readings are correct and filled the object, return true
                if (c == this->chksum[1]) {
                    this->carriagePosition = p;
                    
                    Serial.print("struct iTOW = "); Serial.println(dataSample.iTOW);
                    Serial.print("struct year = "); Serial.println(dataSample.year);
                    Serial.print("struct month = "); Serial.println(dataSample.month);
                    Serial.print("struct day = "); Serial.println(dataSample.day);
                    Serial.print("struct hour = "); Serial.println(dataSample.hour);
                    Serial.print("struct min = "); Serial.println(dataSample.min);
                    Serial.print("struct sec = "); Serial.println(dataSample.sec);
                    
                    
                    return true;
                }
            }

            // Reset the carriage if it is out of packet
            else if (p > (this->size + 4)) {
                p = 0;
            }
        }
    }

    this->carriagePosition = p;

    return false;
}
