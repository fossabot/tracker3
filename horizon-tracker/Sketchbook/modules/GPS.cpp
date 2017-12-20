#include "GPS.h"
#include "../Generic.h"

namespace nmGps {
    HorizonGPS::HorizonGPS()
    {
    }
    
    void HorizonGPS::sendUBX(const byte *MSG, uint8_t len) {
        uint8_t checksumA = 0;
        uint8_t checksumB = 0;
        bool    sent = false;
        SerialGPS.flush();
        SerialGPS.write(0xFF);
        delay(500);
        
        SerialGPS.write(0xB5); // Header bytes
        SerialGPS.write(0x62);
        
        for(uint8_t ubxi = 0; ubxi < len; ubxi++) {
            byte data = pgm_read_byte_near(MSG + ubxi);
            SerialGPS.write(data);
            checksumA += data;
            checksumB += checksumA;
        }
        
        SerialGPS.write(checksumA);
        SerialGPS.write(checksumB);
        delay(500);
    }
    
    bool HorizonGPS::setupGPS(void) {
        INFO;
        Terminal.print(F("Initializing GPS... "));
        LCDprint(F("GPS..."), LEFT, currentLine += LCD_LINE_GAP_SMALL);
        pinMode(POWER_GPS, OUTPUT);
//        digitalWrite(POWER_GPS, LOW);
//        delay(3000);
        digitalWrite(POWER_GPS, HIGH);
        SerialGPS.begin(GPS_BAUD_RATE, SERIAL_8N1);
        ready = false;
        delay(1000);
        //setMode(GPS_HIGH_RATE_MODE);
        
        if(!setupUBX()){
            Terminal.println(F("No GPS detected: check wiring."));
            LCDprint(F("ERROR"), RIGHT, currentLine);
            return false;
        } else {
            ready = true;
            Terminal.println(F("OK!"));
            LCDprint(F("OK!"), RIGHT, currentLine);
            return true;
        }
    }
    
    bool HorizonGPS::setupUBX() {
        bool initialized = true;
        //DISABLE OUTPUT PORTS
        sendUBX(UBX_CFG_PRT_DISABLE, sizeof(UBX_CFG_PRT_DISABLE));
        sendUBX(UBX_CFG_MSG_NAV_PVT_DIS, sizeof(UBX_CFG_MSG_NAV_PVT_DIS));
        sendUBX(UBX_CFG_MSG_ESF_STS_DIS, sizeof(UBX_CFG_MSG_ESF_STS_DIS));
        //ERASE BUFFER
        uint32_t    timeCount = millis();
        while(SerialGPS.available() && ((millis() - timeCount) < TIME_OUT_SERIAL)){
            SerialGPS.read();
        }
        //ENABLE UBX MESSAGES
        sendUBX(UBX_CFG_RATE_1_SEC, sizeof(UBX_CFG_RATE_1_SEC));
        sendUBX(UBX_CFG_NAV5_SET, sizeof(UBX_CFG_NAV5_SET));//STATIC HOLD
        sendUBX(UBX_CFG_MSG_ESF_STS, sizeof(UBX_CFG_MSG_ESF_STS)); //enable esf
        sendUBX(UBX_CFG_MSG_NAV_PVT, sizeof(UBX_CFG_MSG_NAV_PVT));
        sendUBX(UBX_CFG_PRT_9600, sizeof(UBX_CFG_PRT_9600));
        
        return true;//initialized;
    }
    
    bool HorizonGPS::tryGetSerial(){
        if(SerialGPS.available()){
            return true;
        }
        return false;
    }
    
    void HorizonGPS::setMode(int mode){
        if(mode == GPS_POWER_SAVE_MODE){
            sendUBX(UBX_GNSS_DISABLE, sizeof(UBX_GNSS_DISABLE));
            sendUBX(UBX_CFG_PM2_PSMCT____, sizeof(UBX_CFG_PM2_PSMCT____));
            sendUBX(UBX_CFG_RXM_PWR_SAVE, sizeof(UBX_CFG_RXM_PWR_SAVE));
            sendUBX(UBX_CFG_RATE_10_SEC, sizeof(UBX_CFG_RATE_10_SEC));
            if(SHOW_DEBUG){DEBUG;
                Terminal.println(F("HorizonGPS::setMode: Entering GPS power save mode..."));}
        }else if (mode == GPS_LOW_RATE_MODE){
            sendUBX(UBX_GNSS_ENABLED, sizeof(UBX_GNSS_ENABLED));
            sendUBX(UBX_CFG_RXM_NORMAL, sizeof(UBX_CFG_RXM_NORMAL));
            sendUBX(UBX_CFG_RATE_10_SEC, sizeof(UBX_CFG_RATE_10_SEC));
            if(SHOW_DEBUG){DEBUG;
                Terminal.println(F("HorizonGPS::setMode: Entering GPS low rate mode..."));}
        }else if(mode == GPS_MEDIUM_RATE_MODE){
            sendUBX(UBX_GNSS_ENABLED, sizeof(UBX_GNSS_ENABLED));
            sendUBX(UBX_CFG_RXM_NORMAL, sizeof(UBX_CFG_RXM_NORMAL));
            sendUBX(UBX_CFG_RATE_3_SEC, sizeof(UBX_CFG_RATE_3_SEC));
            if(SHOW_DEBUG){DEBUG;
                Terminal.println(F("HorizonGPS::setMode: Entering GPS medium rate mode..."));}
        }else if (mode == GPS_HIGH_RATE_MODE){
            sendUBX(UBX_GNSS_ENABLED, sizeof(UBX_GNSS_ENABLED));
            sendUBX(UBX_CFG_RXM_NORMAL, sizeof(UBX_CFG_RXM_NORMAL));
            sendUBX(UBX_CFG_RATE_1_SEC, sizeof(UBX_CFG_RATE_1_SEC));
            if(SHOW_DEBUG){DEBUG;
                Terminal.println(F("HorizonGPS::setMode: Entering GPS high rate mode..."));}
        }
    }
    
    bool HorizonGPS::encode(){
        unsigned int p = carriagePosition;
        while (SerialGPS.available()) {
            if (p < 2) {
                memset(rawPacket, 0, MESSAGE_BUFFER_SIZE);
                byte c = SerialGPS.read();
                if (c == UBXGPS_HEADER[p]) {
                    p++;
                }else {
                    if(p == 0 && c == NMEAGPS_HEADER[p]){
                        p++;
                    }else if(p == 1 && c == NMEAGPS_HEADER[p] && ready){
                        DEBUG;
                        Terminal.println(F("HorizonGPS::error: NMEA message detected."));
                        gpsAntennaRemoved = true;
                        monitorData.update();
                        setupUBX();
                        p = 0;
                    }else{
                        p = 0;
                    }
                }
            }else if(p < 6){
                byte c = SerialGPS.read();
                if(p==2) headerClass = c;
                if(p==3) headerId = c;
                if(p==4) headerLength = 0x00FF & c;
                if(p==5) headerLength = headerLength + ((0x00FF & c) << 8);
                rawPacket[p-2] = c;
                p++;
            }else {
                
                if(headerLength >= MESSAGE_BUFFER_SIZE){
                    Terminal.println("SIZE OVER FLOW, IGNORING MESSAGE");
                    p = 0;
                    break;
                }

                while (SerialGPS.available() && p < headerLength + 6){
                    rawPacket[p-6 + 4] = SerialGPS.read();
                    p++;
                    this->carriagePosition = p;
                }
                if (p == (headerLength + 6)) {
                    calculateChecksum();
                    p++;
                    carriagePosition = p;

                }else if (p == (headerLength + 7)) {
                    byte c = SerialGPS.read();
                    p++;
                    if (c != chksum[0]) {
                        p = 0;
                        break;
                    }
                }else if (p == (headerLength + 8)) {
                    byte c = SerialGPS.read();
                    p = 0;
                    
                    if (c == chksum[1]) {
                        carriagePosition = p;
                        if(rawPacket[0] == UBX_NAV_PVT[0] && rawPacket[1] == UBX_NAV_PVT[1]){
                            monitorData.updateGpsData();
                        }else if(rawPacket[0] == UBX_ESF_STATUS[0] && rawPacket[1] == UBX_ESF_STATUS[1]){
                            monitorData.updateCalibStatus();
                            break;
//                            esfStatus.print();
                        }else if(rawPacket[0] == UBX_ACK[0] && rawPacket[1] == UBX_ACK[1]){
//                            Terminal.println("RECEIVED ACK");
                            break;
                        }else if(rawPacket[0] == UBX_NAK[0] && rawPacket[1] == UBX_NAK[1]){
//                            Terminal.println("RECEIVED NACK");
                            break;
                        }else{
//                            Terminal.println("RECEIVED UNKNOWN PACKET");
                            break;
                        }
                        return true;
                    }
                }else if(p > (headerLength + 8)) {
                    p = 0;
                }
            }
        }
        carriagePosition = p;
        return false;
    }
    
    void HorizonGPS::calculateChecksum() {
        memset(&chksum[0], 0, 2);
        if(this->headerLength + 4 > 256) Terminal.println("OVER FLOW");
        for (unsigned int i = 0; i < headerLength + 4; i++) {
            chksum[0] += rawPacket[i];
            chksum[1] += chksum[0];
        }
    }
}
