#include "Commands.h"
#include "../Generic.h"

void printLcdData(char *deviceSerial, char *remoteDeviceSerial, char *bluetoothSerial, uint8_t signalPower, bool bluetoothConnected, bool sdCardOk) {
    char lcdOutput[30];
    LCDprint("HORIZON TRACKER", CENTER, currentLine);
    LCDprintNumI(VERSION, CENTER, currentLine+= LCD_LINE_GAP, NULL);
    
    LCDprint("BT: ", LEFT, currentLine+= LCD_LINE_GAP);
    LCDprint(bluetoothSerial, CENTER, currentLine);
    if(bluetoothConnected){
        LCDprint("CON", RIGHT, currentLine);
    }else{
        LCDprint("DES", RIGHT, currentLine);
    }
    
    LCDprint("RF:", LEFT, currentLine+= LCD_LINE_GAP);
    LCDprint(deviceSerial, RIGHT, currentLine);
    
    LCDprint("CT:", LEFT, currentLine+= LCD_LINE_GAP);
    LCDprint(remoteDeviceSerial, RIGHT, currentLine);
    
    if(signalPower > 0){
        LCDprint("CONECTADO", LEFT, currentLine+= LCD_LINE_GAP);
    }else{
        LCDprint("DESCONECTADO", LEFT, currentLine+= LCD_LINE_GAP);
    }
    
    LCDprintNumI(int(-signalPower), RIGHT, currentLine, NULL);
    
    bool gpsLocationValid = (monitorData.gpsStatus & TIME_VALID) && TIME_VALID;
    LCDprint((gpsLocationValid ? "   GPS OK     " : "NO GPS SIGNAL"), RIGHT, currentLine+= LCD_LINE_GAP);
    LCDprint((sdCardOk ? "   SD OK     " : "SD ERROR     "), RIGHT, currentLine+= LCD_LINE_GAP);
    currentLine = INITIAL_LINE;
}

void updateLcdData(uint8_t signalPower, bool bluetoothConnected){
    
    if(bluetoothConnected){
        LCDprint("CON", RIGHT, currentLine+= (2*LCD_LINE_GAP));
    }else{
        LCDprint("DES", RIGHT, currentLine+= (2*LCD_LINE_GAP));
    }
    
    if(signalPower > 0){
        LCDprint("CONECTADO   ", LEFT, currentLine+= (3*LCD_LINE_GAP));
    }else{
        LCDprint("DESCONECTADO", LEFT, currentLine+= (3*LCD_LINE_GAP));
    }
    
    LCDprintNumI(int(-signalPower), RIGHT, currentLine, NULL);
    
    bool gpsLocationValid = (monitorData.gpsStatus & TIME_VALID) && TIME_VALID;
    LCDprint((gpsLocationValid ? "   GPS OK     " : "NO GPS SIGNAL"), RIGHT, currentLine+= LCD_LINE_GAP);
    LCDprint((nmSdCard::isOk() ? "   SD OK     " : "SD ERROR     "), RIGHT, currentLine+= LCD_LINE_GAP);
    
    currentLine = INITIAL_LINE;
    
}


bool analyseXbeePack(){
    if(SHOW_TODO){TODO;
        Terminal.println(F("newPacket: check packet parameters"));}
    if(SHOW_WARNING){WARNING;
        Terminal.println(F("newPacket: implements only transmit packets, may return 0 if packet is not recognized"));}
    
    if(horizonXbee.xbeePacket.getPacketSourceEndpoint() >= LOWER_SOURCE_ENDPOINT_ORB && horizonXbee.xbeePacket.getPacketSourceEndpoint() <= UPPER_SOURCE_ENDPOINT_ORB){
        char tmp[ORB_HEADER_SIZE + horizonXbee.xbeePacket.getLength() - EXP_RX_PKT_HEADER_SIZE];
        memset(tmp, 0, sizeof(tmp));
        memcpy(tmp, horizonXbee.xbeePacket.getPacketRemoteAddress(), 8);
        tmp[8] = horizonXbee.xbeePacket.getPacketSourceEndpoint();
        tmp[9] = horizonXbee.xbeePacket.getPacketDestinationEndpoint();
        tmp[10] = byte(horizonXbee.xbeePacket.getPacketClusterId() >> 8);
        tmp[11] = byte(horizonXbee.xbeePacket.getPacketClusterId() & 0x00FF);
        tmp[12] = byte(horizonXbee.xbeePacket.getPacketProfileId() >> 8);
        tmp[13] = byte(horizonXbee.xbeePacket.getPacketProfileId() & 0x00FF);
        memcpy(&tmp[14], horizonXbee.xbeePacket.getPacketPayload(), horizonXbee.xbeePacket.getLength() - EXP_RX_PKT_HEADER_SIZE);
        horizonBluetooth.newPacket(CMD_XBEE_PAYLOAD, tmp, sizeof(tmp));
        horizonBluetooth.sendMessage();
        
    }else if(horizonXbee.xbeePacket.getPacketSourceEndpoint() == SOURCE_ENDPOINT_CMD){
        if(horizonXbee.xbeePacket.getPacketDestinationEndpoint() == CMD_RESET){
            horizonXbee.newPacket(PACKET_EXPLICIT_ADDRESS_CMD_FRAME, NULL, horizonXbee.xbeePacket.getPacketRemoteAddress(), 2, SOURCE_ENDPOINT_DEFAULT, 0, 0, 0, "OK", 2);
            horizonXbee.sendPacket();
            softwareReset();
        }else if(horizonXbee.xbeePacket.getPacketDestinationEndpoint() == CMD_UPDATE){
            wdt_disable();
            int verifiedFirmware = verifyFirmware();
            if(verifiedFirmware == FW_OK) {
                EEPROM.write(0x1FF, 0xF0);
                horizonXbee.newPacket(PACKET_EXPLICIT_ADDRESS_CMD_FRAME, NULL, horizonXbee.xbeePacket.getPacketRemoteAddress(), 2, SOURCE_ENDPOINT_DEFAULT, 0, 0, 0, "OK", 2);
                horizonXbee.sendPacket();
                wdt_enable(WDTO_250MS); // have the wdt reset the chip
                wdt_reset();
                delay(1000); // wait 10000ms to timeout wdt
            }else if(verifiedFirmware == FW_CORRUPTED){
                wdt_enable(WDTO_8S);
                horizonXbee.newPacket(PACKET_EXPLICIT_ADDRESS_CMD_FRAME, NULL, horizonXbee.xbeePacket.getPacketRemoteAddress(), 2, SOURCE_ENDPOINT_DEFAULT, 0, 0, 0, "ERROR_FW_CORRUPTED", 18);
                horizonXbee.sendPacket();
            }else if(verifiedFirmware == MISSING_SD){
                wdt_enable(WDTO_8S);
                horizonXbee.newPacket(PACKET_EXPLICIT_ADDRESS_CMD_FRAME, NULL, horizonXbee.xbeePacket.getPacketRemoteAddress(), 2, SOURCE_ENDPOINT_DEFAULT, 0, 0, 0, "ERROR_NO_SDCARD", 15);
                horizonXbee.sendPacket();
            }else if(verifiedFirmware == FW_OUTOFDATE){
                wdt_enable(WDTO_8S);
                horizonXbee.newPacket(PACKET_EXPLICIT_ADDRESS_CMD_FRAME, NULL, horizonXbee.xbeePacket.getPacketRemoteAddress(), 2, SOURCE_ENDPOINT_DEFAULT, 0, 0, 0, "ERROR_FW_OUTOFDATE", 18);
                horizonXbee.sendPacket();
            }else {
                wdt_enable(WDTO_8S);
            }
            wdt_disable();
            delay(1000);
            softwareReset();
            
        }else if(horizonXbee.xbeePacket.getPacketDestinationEndpoint() == CMD_TX_ENABLE){
            horizonXbee.newPacket(PACKET_EXPLICIT_ADDRESS_CMD_FRAME, NULL, horizonXbee.xbeePacket.getPacketRemoteAddress(), 2, SOURCE_ENDPOINT_DEFAULT, 0, 0, 0, "OK", 2);
            horizonXbee.sendPacket();
            enableTx = true;
        }else if(horizonXbee.xbeePacket.getPacketDestinationEndpoint() == CMD_TX_DISABLE){
            horizonXbee.newPacket(PACKET_EXPLICIT_ADDRESS_CMD_FRAME, NULL, horizonXbee.xbeePacket.getPacketRemoteAddress(), 2, SOURCE_ENDPOINT_DEFAULT, 0, 0, 0, "OK", 2);
            horizonXbee.sendPacket();
            enableTx = false;
        }else if(horizonXbee.xbeePacket.getPacketDestinationEndpoint() == CMD_OFFLINE_DATA){
            DEBUG;
            Terminal.println("DISABLE OFFLINE DATA TRANSMISSION");
            horizonXbee.newPacket(PACKET_EXPLICIT_ADDRESS_CMD_FRAME, NULL, horizonXbee.xbeePacket.getPacketRemoteAddress(), 2, SOURCE_ENDPOINT_DEFAULT, 0, 0, 0, "OK", 2);
            horizonXbee.sendPacket();
            enableOfflineDataSend = false;
            offlineDataDisableAt = millis();
        }else if(horizonXbee.xbeePacket.getPacketDestinationEndpoint() == CMD_ALIVE){
            DEBUG;
            Terminal.println("Central is alive");
            centralAliveAt = millis();
            horizonConnected = true;
            horizonXbee.setDefaultDestinationAddress(horizonXbee.xbeePacket.getPacketRemoteAddress());
            printAsHexa(horizonXbee.getDestinationAddress(), 8);
        }else {
            if(SHOW_ERROR){ERROR;
                Terminal.println(F("Commands::analyseXbeePack: Invalid command"));}
        }
    }
    return true;
}


bool OrbBluetoothHeader::parse(char *data){
    memcpy(packetRemoteAddress, data, 8);
    packetSourceEndpoint = data[8];
    packetDestinationEndpoint = data[9];
    packetClusterId = ((uint16_t(data[10]) << 8) & 0xFF00) + (data[11] & 0x00FF);
    packetProfileId = ((uint16_t(data[12]) << 8) & 0xFF00) + (data[13] & 0x00FF);
    memcpy(packetUUID, &data[14], 16);
    return true;
}

void OrbBluetoothHeader::toArray(char *rawPacket){
    memset(rawPacket, 0, 30);
    memcpy(rawPacket, packetRemoteAddress, 8);
    rawPacket[8] = packetSourceEndpoint;
    rawPacket[9] = packetDestinationEndpoint;
    rawPacket[10] = packetClusterId >> 8;
    rawPacket[11] = packetClusterId & 0x00FF;
    rawPacket[12] = packetProfileId >> 8;
    rawPacket[13] = packetProfileId & 0x00FF;
    memcpy(&rawPacket[14], packetUUID, 16);
}

bool saveLog(char *logText, unsigned long fileNumber){
    char filePath[24];
    memset(filePath, 0, 24);
    sprintf_P(filePath, PSTR("%s/%08lX.TXT"),LOG_DIR,fileNumber);
    if(SHOW_DEBUG){DEBUG;
        Terminal.println(F("savelog: New log file created."));
        Terminal.println(filePath);}
    if(!nmSdCard::writeSDline(filePath, logText)){
        if(SHOW_ERROR){ERROR;
            Terminal.println(F("savelog: failed to write log file, check the SD card"));}
        return false;
    }else {
        if(SHOW_INFO){INFO;
            Terminal.println(F("savelog: log file successfully saved"));}
        char lastFileName[16];
        memset(lastFileName, 0, 16);
        sprintf_P(lastFileName, PSTR("%08lX"),fileNumber);
        nmSdCard::overwriteSDline(STATUS_FILE, lastFileName);
    }
    return true;
}


int verifyFirmware() {
    char *tmp;
    int i = 0;
    uint32_t crc32;
    uint32_t lengthInformed = 0;
    uint32_t newVersion = 0;
    int result = 0;
    LCD_ON;
    currentLine = INITIAL_LINE;
    LCDprint(F("FIRMWARE UPDATE"), CENTER, currentLine);
    if(!nmSdCard::isOk()) {
        LCDprint("ABORT: insert SD", LEFT, currentLine+=LCD_LINE_GAP);
        LCD_OFF;
        return MISSING_SD;
    }
    tmp = strtok(horizonXbee.xbeePacket.getPacketPayload(), "_");
    while(tmp && i < 3) {
        if(i == 0) {
            if(strtoul(tmp, NULL, DEC) <= VERSION){
                LCD_OFF;
                return FW_OUTOFDATE;
            }
        }else if(i == 1) {
            lengthInformed = strtoul(tmp, NULL, DEC);
        }else if(i == 2) {
            crc32 = strtoul(tmp, NULL, HEX);
        }
        i++;
        tmp = strtok(NULL, "_");
    }
    
    LCDprint("Receive file...", LEFT, currentLine+=LCD_LINE_GAP);
    nmSdCard::remove(FW_FILENAME);
    char destAdd[17];
    memset(destAdd, 0, 17);
    toHexaString(destAdd, horizonXbee.xbeePacket.getPacketRemoteAddress(), 8);
    Terminal.println(destAdd);
    horizonXbee.setDestinationAddress(destAdd);
    horizonXbee.setMode(TRANSPARENT_MODE);
    horizonXbee.newPacket(PACKET_TX_REQUEST, NULL, NULL, NULL, NULL, NULL, NULL, NULL, "START_TRANSFER", 14);
    
    horizonXbee.sendPacket();
    XReceive(&SD, &SerialXBee, FW_FILENAME); // Receive XMODEM and overwrite if file exists -> HOST WILL RECEIVE CHARACTER 'C' 0x43 AS START FLAG
    
    File f = SD.open(FW_FILENAME);
    if(f) {
        uint32_t crc32calculated = fileCRC32fixedLength(f, lengthInformed);
        f.close();
        if(crc32 != crc32calculated) {
            LCDprint("ERROR", LEFT, currentLine+=LCD_LINE_GAP);
            SD.remove(FW_FILENAME); // Remove FIRMWARE.BIN
            result = FW_CORRUPTED;
        }else{
            LCDprint("OK", LEFT, currentLine+=LCD_LINE_GAP);
        }
    }else {
        LCDprint("ERROR", LEFT, currentLine+=LCD_LINE_GAP);
        result = MISSING_SD;
    }
    delay(1000);
    horizonXbee.setMode(API_MODE);
    delay(1000);
    LCD_OFF;
    return result;
}

void copyFileQueue(char *logFileName){
    unsigned long position = 0;
    if(nmSdCard::exists(FILE_POSITION)){
        char lastPosition[16];
        memset(lastPosition, 0, 16);
        nmSdCard::readLine(FILE_POSITION, lastPosition);
        position = strtoul(lastPosition, NULL, HEX);
    }
    File file = SD.open(logFileName, FILE_READ);
    if(file) {
        file.seek(position);
        if(file.available() && !file.isDirectory()){
            String readLine = file.readStringUntil('\n');
            readLine.concat("\n");
            saveLog(readLine.c_str(), logFileNumber);
            logFileNumber ++;
            position = file.position();
            char lastPosition[16];
            memset(lastPosition, 0, 16);
            sprintf_P(lastPosition, PSTR("%08lX"), position);
            nmSdCard::overwriteSDline(FILE_POSITION, lastPosition);
        }else{
            if(file.isDirectory()){
                Terminal.println("REBOOT");
                LCD_ON;
                currentLine = INITIAL_LINE;
                LCDprint("SD CARD FAIL", CENTER, currentLine);
                LCDprint("REBOOT", CENTER, currentLine+= LCD_LINE_GAP);
                file.close();
                nmSdCard::remove(FILE_POSITION);
                wdt_enable(WDTO_250MS); // have the wdt reset the chip
                wdt_reset();
                delay(1000); // wait 10000ms to timeout wdt
            }
            file.close();
            nmSdCard::remove(FILE_POSITION);
            nmSdCard::remove(logFileName);
            return;
        }
        file.close();
        return;
    }
}
