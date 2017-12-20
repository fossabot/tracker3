#include "XBee.h"
#include "../Generic.h"

namespace nmXbee{
    HorizonXbee::HorizonXbee()
    :programable(false)
    ,operatingMode(API_MODE)
    ,powerLevel(0)
    ,packetStored(false)
    {
        memset(deviceAddress, 0, sizeof(deviceAddress));
        memset(deviceDestinationAddress, 0, sizeof(deviceDestinationAddress));
        memcpy(deviceAddress, "00000000000000",16);
        long high = strtoul(String(XBEE_BROADCAST_ADD).substring(0, 8).c_str(), NULL, 16);
        long low = strtoul(String(XBEE_BROADCAST_ADD).substring(8, 16).c_str(), NULL, 16);
        ltoba(high, deviceDestinationAddress);
        ltoba(low, &deviceDestinationAddress[4]);
    }
    
    bool HorizonXbee::setupXBee(char *destinationAddress, char mode) {
        INFO;Terminal.println(F("Initializing XBee... "));
        LCDprint(F("Radio..."), LEFT, currentLine += LCD_LINE_GAP);
        
        digitalWrite(POWER_XBEE, HIGH);
        delay(500);
        SerialXBee.begin(XBEE_BAUD_RATE);
        delay(100);
        
        char atResponse[AT_BUFFER_ZISE];
        memset(atResponse, 0, AT_BUFFER_ZISE);
        bool xbeeInitialized = true;
        
        if(programable){
            SerialXBee.print(F("B")); // Choose bootloader "B" Serial bypass mode
        }
        
        if(SHOW_INFO){INFO;
            Terminal.print(F("Entering AT mode..."));}
        
        uint32_t timeCount = millis();
        while(!enterAtMode() && (millis() - timeCount) < 15000);
        
        INFO;Terminal.print(F("Reading XBee Terminal... "));
        
        xbeeInitialized &= sendAtCommand(AT_SERIAL_HIGH, atResponse);
        memset(deviceAddress, '0', 16);
        memcpy(deviceAddress + 8 - strlen(atResponse), atResponse, strlen(atResponse));
        
        memset(atResponse, 0, AT_BUFFER_ZISE);
        xbeeInitialized &= sendAtCommand(AT_SERIAL_LOW, atResponse);
        memcpy(deviceAddress + 16 - strlen(atResponse), atResponse, strlen(atResponse));
        
        Terminal.println(deviceAddress);
        INFO;Terminal.print(F("Setting XBee Destination Terminal... "));
        
        
        if(strlen(destinationAddress)<16){
            ERROR;Terminal.println(F("Invalid destination address setting to broadcast."));
        }else {
            long high = strtoul(String(destinationAddress).substring(0, 8).c_str(), NULL, 16);
            long low = strtoul(String(destinationAddress).substring(8, 16).c_str(), NULL, 16);
            char tmp[4];
            ltoba(high, tmp);
            memcpy(deviceDestinationAddress, tmp, 4);
            ltoba(low, tmp);
            memcpy(&deviceDestinationAddress[4], tmp, 4);
        }
        printAsHexa(deviceDestinationAddress, 8);
        
        char destAdd[20];
        toHexaString(destAdd, getDestinationAddress(), 8);
        
        xbeeInitialized &= sendAtCommand(AT_DST_SERIAL_HIGH + String(destAdd).substring(0,8), NULL);
        xbeeInitialized &= sendAtCommand(AT_DST_SERIAL_LOW + String(destAdd).substring(8), NULL);
        
        INFO;Terminal.println(F("Setting API Mode..."));
        xbeeInitialized &= sendAtCommand(AT_MODE + String(operatingMode),NULL);
        xbeeInitialized &= sendAtCommand(AT_TX_OPTIONS_DIGIMESH,NULL);
        xbeeInitialized &= sendAtCommand(AT_EXPLICIT_MODE,NULL);
        
        INFO;Terminal.println(F("Exiting AT mode..."));
        xbeeInitialized &= sendAtCommand(EXIT_AT_CMD_MODE,NULL);
        
        if(xbeeInitialized) {
            INFO;Terminal.println(F("XBee initialized in serial bypass mode. "));
            INFO;Terminal.print(F("Checking connection status... "));
            newPacket(PACKET_EXPLICIT_ADDRESS_CMD_FRAME, NULL, deviceDestinationAddress, 2, 0, 0, 0, 0, "TESTE", 5);
            sendPacket();
            
            Terminal.println(online ? F("CONNECTED!") : F("NOT CONNECTED"));
            LCDprint(online ? F("ONLINE") : F("OFFLINE"), RIGHT, currentLine);
            LCDprint(deviceAddress, LEFT, currentLine += LCD_LINE_GAP);
        } else {
            Terminal.println(F("ERROR"));
            LCDprint(F("ERROR"), RIGHT, currentLine);
        }
        return xbeeInitialized;
    }
    
    bool HorizonXbee::enterAtMode(void) {
        uint32_t timeCount;
        SerialXBee.print(ENTER_AT_CMD_MODE);
        timeCount = millis();
        while(!SerialXBee.available() && ((millis() - timeCount) < TIME_OUT_SERIAL));
        if(strcasestr(SerialXBee.readStringUntil(CR).c_str(), OK)) {
            return true;
        }
        return false;
    }
    
    bool HorizonXbee::sendAtCommand(String cmd, char * response) {
        uint32_t timeCount = millis();
        bool responseReceived = false;
        int iterator = 0;
        char readByte;
        if(SHOW_DEBUG){DEBUG;
            Terminal.println(F("HorizonXbee::sendAtCommand: Xbee AT: "));
            Terminal.println(cmd);}
        SerialXBee.println(cmd);
        while(!SerialXBee.available() && ((millis() - timeCount) < TIME_OUT_AT));
        while(SerialXBee.available()) {
            readByte = SerialXBee.read();
            if(readByte == CR){
                break;
            }
            if(response != NULL && iterator < AT_BUFFER_ZISE){
                response[iterator++] = readByte;
            }
            responseReceived = true;
            timeCount = millis();
            while(!SerialXBee.available() && ((millis() - timeCount) < TIME_OUT_SERIAL));
        }
        if(SHOW_DEBUG){DEBUG;
            Terminal.println(F("HorizonXbee::sendAtCommand: Xbee AT response: "));
            printAsHexa(response, iterator);}
        
        return responseReceived;
    }
    
    bool HorizonXbee::setDestinationAddress(char *address){
        bool addOk = true;
        Terminal.println(address);
        if(strlen(address)<16){
            if(SHOW_ERROR){ERROR;
                Terminal.println(F("HorizonXbee::setDestinationAddress: Invalid destination address."));}
            addOk = false;
        }else {
            uint32_t timeCount = millis();
            while(!enterAtMode() && (millis() - timeCount) < 15000);
            Terminal.println(F("HorizonXbee::setDestinationAddress: setting destination address."));
            addOk &= sendAtCommand(AT_DST_SERIAL_HIGH + String(address).substring(0,8), NULL);
            addOk &= sendAtCommand(AT_DST_SERIAL_LOW + String(address).substring(8), NULL);
            addOk &= sendAtCommand(EXIT_AT_CMD_MODE,NULL);

        }
        return addOk;
    }
    
    bool HorizonXbee::setDefaultDestinationAddress(char *address){
        memcpy(deviceDestinationAddress, address, 8);
    }
    
    bool HorizonXbee::setMode(char mode){
        bool modeOk = true;
        uint32_t timeCount = millis();
        while(!enterAtMode() && (millis() - timeCount) < 15000);
        modeOk &= sendAtCommand(AT_MODE + String(mode),NULL);
        modeOk &= sendAtCommand(EXIT_AT_CMD_MODE,NULL);
        if(modeOk){
            operatingMode = mode;
        }
        return modeOk;
    }
    
    bool HorizonXbee::tryGetSerial(){
        if(SerialXBee.available()){
            return true;
        }return false;
    }
    
    bool HorizonXbee::getXbeePacket(){
        uint32_t    timeCount = millis();
        char        localMessageBuffer[MESSAGE_BUFFER_SIZE];
        int         receivedCount = 0;
        byte        readByte = 0;
        bool        messageStart = false;
        bool        messageReceived = false;
        int         length = 0;
        uint8_t     frameType = 0;
        memset(localMessageBuffer, 0, MESSAGE_BUFFER_SIZE);
        
        while (SerialXBee.available()) {
            if(messageStart){
                if(receivedCount == PKT_SIZE_OFFSET){
                    readByte = SerialXBee.read();
                    length = readByte;
                }
                if(receivedCount == PKT_SIZE_OFFSET+1){
                    readByte = SerialXBee.read();
                    length = (length<<8)+readByte;
                }
                if(receivedCount == PKT_TYPE_OFFSET){
                    readByte = SerialXBee.read();
                    frameType = readByte;
                }
                
                if(receivedCount > PKT_TYPE_OFFSET){
                    if(SerialXBee.available() >= length){
                        receivedCount = receivedCount + SerialXBee.readBytes(&localMessageBuffer[receivedCount],length);
                        
                        if(checksum(&localMessageBuffer[PKT_TYPE_OFFSET],receivedCount-PKT_HEADER_SIZE) > 0){
                            //if(SHOW_INFO){INFO;
                            Terminal.println(F("HorizonXbee::getXbeePacket 0: wrong checksum, message corrupted."));//}
                            break;
                        }
                        messageReceived = true;
                        break;
                    }else{
                        timeCount = millis();
                        while(!(SerialXBee.available() >= length) && (millis() >= timeCount) && ((millis() - timeCount) < TIME_OUT_SERIAL));
                        if(SerialXBee.available() >= length){
                            continue;
                        }else{
                            //if(SHOW_INFO){INFO;
                            Terminal.println(F("HorizonXbee::getXbeePacket 1: time out, message not received."));//}
                            break;
                        }
                    }
                }
                
                timeCount = millis();
                while(!SerialXBee.available() && ((millis() - timeCount) < TIME_OUT_SERIAL));
                if(!SerialXBee.available()){
                    //if(SHOW_INFO){INFO;
                    Terminal.println(F("HorizonXbee::getXbeePacket 2: time out, message not received."));//}
                    break;
                }
            }else {
                readByte = SerialXBee.read();
                if(readByte == START_DELIMITER){
                    messageStart = true;
                    receivedCount = 0;
                }
                timeCount = millis();
                while(!SerialXBee.available() && ((millis() - timeCount) < TIME_OUT_SERIAL));
                if(!SerialXBee.available()){
                    //if(SHOW_INFO){INFO;
                    Terminal.println(F("HorizonXbee::getXbeePacket 3: time out, message not received."));//}
                    break;
                }
            }
            if(receivedCount < MESSAGE_BUFFER_SIZE) {
                localMessageBuffer[receivedCount++] = readByte;
            }else if (messageStart){
                //if(SHOW_ERROR){ERROR;
                Terminal.println(F("HorizonXbee::getXbeePacket: Insufficient memory allocated."));//}
                //if(SHOW_TODO){TODO;
                Terminal.println(F("HorizonXbee::getXbeePacket: check message size before reading it."));//}
                break;
            }
            
        }
        if(messageReceived){
            xbeePacket.parse(localMessageBuffer, receivedCount);
        }
        return messageReceived;
    }
    
    
    bool HorizonXbee::sendPacket() {
        
        char tmp[MESSAGE_BUFFER_SIZE];
        int size = xbeePacket.toArray(tmp);
        uint32_t timeCount = millis();
        bool packetSent = false;
        
        packetStored = false;
        char remoteAdd[17];
        if(operatingMode == TRANSPARENT_MODE){
            Terminal.println("SEND DATA");
            Terminal.println(xbeePacket.getPacketPayload());
            SerialXBee.print(xbeePacket.getPacketPayload());
            packetSent = true;
        }else {
            toHexaString(remoteAdd, xbeePacket.getPacketRemoteAddress(), 8);
            SerialXBee.write(tmp,size);
            while(!SerialXBee.available() && (millis() - timeCount) < PACKET_SENT_TIME_OUT);
            if(tryGetSerial()){
                getXbeePacket();
                if(xbeePacket.getFrameType() == PACKET_TX_STATUS){
                    if(xbeePacket.getPacketStatus() == TRANSMIT_SUCCESS){
                        packetSent = true;
                    }else {
                        if(SHOW_INFO){INFO;
                            Terminal.println(F("COULD NOT SEND PACKET"));}
                        packetSent = false;
                        powerLevel = 0;
                    }
                }else {
                    //if(SHOW_INFO){INFO;
                    Terminal.println(F("PACKET RECEIVED IS NOT STATUS"));
                    TODO;
                    Terminal.println(F("Save received packet for post analysis"));//}
                    packetStored = true;
                }
            }else{
                if(SHOW_INFO){INFO;
                    Terminal.println(F("NO RESPONSE RECEIVED"));}
                packetSent = false;
                powerLevel = 0;
            }
        }
//        if(packetSent){
            char destAdd[17];
            toHexaString(destAdd, getDestinationAddress(), 8);
            if(strcmp(destAdd, remoteAdd) == 0){
                online = packetSent;
            }
//        }
        return packetSent;
    }
    
    void HorizonXbee::newPacket(uint8_t frameType, char *command, char *remoteAddress, uint8_t frameId, uint8_t sourceEndpoint, uint8_t destinationEndpoint, uint16_t clusterId, uint16_t profileId, char *data, int dataSize){
        if(SHOW_TODO){TODO;
            Terminal.println(F("newPacket: check packet parameters"));}
        if(SHOW_WARNING){WARNING;
            Terminal.println(F("newPacket: implements only transmit packets, may return 0 if packet is not recognized"));}
        xbeePacket.clear();
        xbeePacket.packetFrameType = frameType;
        xbeePacket.packetFrameId = frameId;
        
        if(xbeePacket.packetFrameType == PACKET_AT_CMD){
            memcpy(xbeePacket.packetCommand, command, 2);
            memcpy(xbeePacket.packetPayload, data, dataSize);
            xbeePacket.packetLength = dataSize + AT_CMD_HEADER_SIZE;
            
        }else if (xbeePacket.packetFrameType == PACKET_TX_REQUEST){
            memcpy(xbeePacket.packetPayload, data, dataSize);
            xbeePacket.packetLength = dataSize + TX_HEADER_SIZE;
            
        }else if (xbeePacket.packetFrameType == PACKET_EXPLICIT_ADDRESS_CMD_FRAME){
            memcpy(xbeePacket.packetRemoteAddress, remoteAddress, 8);
            memcpy(xbeePacket.packetPayload, data, dataSize);
            xbeePacket.packetSourceEndpoint = sourceEndpoint;
            xbeePacket.packetDestinationEndpoint = destinationEndpoint;
            xbeePacket.packetClusterId = clusterId;
            xbeePacket.packetProfileId = profileId;
            xbeePacket.packetLength = dataSize + EXP_TX_HEADER_SIZE;
        }else{
            if(SHOW_ERROR){ERROR;
                Terminal.println(F("newPacket: Unrecognized packet type. Packet may be valid, library implementation is incomplete."));}
        }
    }
    
    void HorizonXbee::setPacketStored(bool isStored){
        packetStored = isStored;
    }
    
    bool HorizonXbee::readPowerLevel(){
        char tmp[MESSAGE_BUFFER_SIZE];
        int size;
        uint32_t timeCount = millis();
        newPacket(PACKET_EXPLICIT_ADDRESS_CMD_FRAME, NULL, deviceDestinationAddress, 2, 0, 0, 0, 0, "TESTE", 5);
        if(sendPacket()){
            newPacket(PACKET_AT_CMD, LAST_PACKET_RSSI, NULL, 2, NULL, NULL, NULL, NULL, NULL, 0);
            memset(tmp, 0, MESSAGE_BUFFER_SIZE);
            size = xbeePacket.toArray(tmp);
            SerialXBee.write(tmp, size);
            timeCount = millis();
            while(!SerialXBee.available() && (millis() - timeCount) < TIME_OUT_SERIAL);
            if(tryGetSerial()){
                getXbeePacket();
                if(xbeePacket.getFrameType() == PACKET_AT_CMD_RESPONSE){
                    powerLevel = xbeePacket.getPacketPayload()[0];
                }else {
                    if(SHOW_INFO){INFO;
                        Terminal.println(F("PACKET REEIVED IS NOT AT RESPONSE"));}
                    packetStored = true;
                }
            }else {
                if(SHOW_INFO){INFO;
                    Terminal.println(F("AT RESPONSE NOT RECEIVED"));}
            }
        }else{
            powerLevel = 0;
        }
        
    }
    
    //XbeePacket
    bool XbeePacket::parse(char *data, int size){
        if(SHOW_TODO){TODO;
            Terminal.println(F("XbeePacket::parse: check packet before parsing"));}
        if(SHOW_WARNING){WARNING;
            Terminal.println(F("XbeePacket::parse: minimal necessary implementation, may lost some data"));}
        clear();
        packetLength = (((data[PKT_SIZE_OFFSET] << 8) & 0xFF00) + (data[PKT_SIZE_OFFSET+1] & 0x00FF));
        packetFrameType = data[PKT_TYPE_OFFSET];
        memset(packetPayload, 0, MESSAGE_BUFFER_SIZE);
        packetChecksum = data[size-1];
        if(packetFrameType == PACKET_TX_STATUS){
            packetStatus = data[TX_STS_PCKT_STATUS_OFFSET];
        }else if(packetFrameType == PACKET_RX_PACKET){
            memcpy(packetRemoteAddress, &data[RX_PKT_ADD_OFFSET], 8);
            memcpy(packetPayload, &data[RX_PKT_DATA_OFFSET], packetLength - RX_PKT_HEADER_SIZE);
            
        }else if(packetFrameType == PACKET_EXPLICIT_RX_INDICATOR){
            memcpy(packetRemoteAddress, &data[RX_PKT_ADD_OFFSET], 8);
            packetSourceEndpoint = data[EXP_RX_PKT_SRC_EP_OSSSET];
            packetDestinationEndpoint = data[EXP_RX_PKT_DST_EP_OSSSET];
            packetClusterId = ((uint16_t(data[EXP_RX_PKT_CLUSTERID_OFFSET]) << 8) & 0xFF00) + (data[EXP_RX_PKT_CLUSTERID_OFFSET+1] & 0x00FF);
            packetProfileId = ((uint16_t(data[EXP_RX_PKT_PROFILEID_OFFSET]) << 8) & 0xFF00) + (data[EXP_RX_PKT_PROFILEID_OFFSET+1] & 0x00FF);
            memcpy(packetPayload, &data[EXP_RX_PKT_DATA_OFFSET], packetLength - EXP_RX_PKT_HEADER_SIZE);
            
        }else if (packetFrameType == PACKET_AT_CMD_RESPONSE){
            memcpy(packetCommand, &data[AT_CMD_RESP_PKT_CMD_OFFSET], 2);
            packetStatus = data[AT_CMD_RESP_PKT_STATUS_OFFSET];
            memcpy(packetPayload, &data[AT_CMD_RESP_PKT_DATA_OFFSET], packetLength - AT_CMD_RESP_PKT_HEADER_SIZE);
            
        }else {
            if(SHOW_ERROR){ERROR;
                Terminal.println(F("XbeePacket::parse: Unrecognized packet type. Packet may be valid, library implementation is incomplete."));}
            return false;
        }
        return true;
    }
    
    int XbeePacket::toArray(char *rawPacket){
        if(SHOW_TODO){TODO;
            Terminal.println(F("XbeePacket::toArray: check packet"));}
        if(SHOW_WARNING){WARNING;
            Terminal.println(F("XbeePacket::toArray: implements only transmit packets, may return 0 if packet is not recognized"));}
        rawPacket[PKT_DELIMITER_OFFSET] = START_DELIMITER;
        rawPacket[PKT_SIZE_OFFSET] = packetLength >> 8;
        rawPacket[PKT_SIZE_OFFSET+1] = packetLength & 0x00FF;
        rawPacket[PKT_TYPE_OFFSET] = packetFrameType;
        
        
        if(packetFrameType == PACKET_AT_CMD){
            rawPacket[PKT_FRAMEID_OFFSET] = packetFrameId;
            memcpy(&rawPacket[AT_CMD_COMMAND_OFFSET], packetCommand, 2);
            memcpy(&rawPacket[AT_CMD_DATA_OFFSET], packetPayload, packetLength - AT_CMD_HEADER_SIZE);
            rawPacket[PKT_HEADER_SIZE + packetLength] = checksum(&rawPacket[3], packetLength);
            
        }else if (packetFrameType == PACKET_TX_REQUEST){
            rawPacket[PKT_FRAMEID_OFFSET] = packetFrameId;
            memcpy(&rawPacket[PKT_DESTINATION_ADD_OFFSET], packetRemoteAddress, 8);
            //Reserved fields in packet
            rawPacket[13] = 0xFF;
            rawPacket[14] = 0xFE;
            rawPacket[15] = 0x00; //Broadcast radius set to maximum.
            rawPacket[16] = TX_OPTIONS_DIGIMESH; //Application always use digimesh.
            memcpy(&rawPacket[TX_DATA_OFFSET], packetPayload, packetLength - TX_HEADER_SIZE);
            rawPacket[PKT_HEADER_SIZE + packetLength] = checksum(&rawPacket[3], packetLength);
            
        }else if (packetFrameType == PACKET_EXPLICIT_ADDRESS_CMD_FRAME){
            rawPacket[PKT_FRAMEID_OFFSET] = packetFrameId;
            memcpy(&rawPacket[PKT_DESTINATION_ADD_OFFSET], packetRemoteAddress, 8);
            //Reserved fields in packet
            rawPacket[13] = 0xFF;
            rawPacket[14] = 0xFE;
            rawPacket[EXP_TX_SRC_EP_OFFSET] = packetSourceEndpoint;
            rawPacket[EXP_TX_DST_EP_OFFSET] = packetDestinationEndpoint;
            rawPacket[EXP_TX_CLUSTERID_OFFSET] = packetClusterId >> 8;
            rawPacket[EXP_TX_CLUSTERID_OFFSET+1] = packetClusterId & 0x00FF;
            rawPacket[EXP_TX_PROFILEID_OFFSET] = packetProfileId >> 8;
            rawPacket[EXP_TX_PROFILEID_OFFSET+1] = packetProfileId & 0x00FF;
            rawPacket[21] = 0x00; //Broadcast radius set to maximum.
            rawPacket[22] = TX_OPTIONS_DIGIMESH; //Application always use digimesh.
            memcpy(&rawPacket[EXP_TX_DATA_OFFSET], packetPayload, packetLength - TX_HEADER_SIZE);
            rawPacket[PKT_HEADER_SIZE + packetLength] = checksum(&rawPacket[3], packetLength);
        }else{
            if(SHOW_ERROR){ERROR;
                Terminal.println(F("XbeePacket::toArray: Unrecognized packet type. Packet may be valid, library implementation is incomplete."));}
            return 0;
        }
        return packetLength + 4;
    }
    
    void XbeePacket::print(){
        if(SHOW_TODO){TODO;
            Terminal.println(F("XbeePacket::print: check packet"));}
        
        Terminal.println(F("Xbee packet"));
        Terminal.print(F("Packet length: "));
        Terminal.println(packetLength, HEX);
        Terminal.print(F("Frame type: "));
        Terminal.println(packetFrameType, HEX);
        
        if(packetFrameType == PACKET_TX_STATUS){
            Terminal.print(F("Status: "));
            Terminal.println(packetStatus,HEX);
        }else if(packetFrameType == PACKET_RX_PACKET){
            Terminal.print(F("Remote address: "));
            printAsHexa(packetRemoteAddress, 8);
            Terminal.print(F("Payload: "));
            printAsHexa(packetPayload,packetLength - RX_PKT_HEADER_SIZE);
            
        }else if(packetFrameType == PACKET_EXPLICIT_RX_INDICATOR){
            Terminal.print(F("Remote address: "));
            printAsHexa(packetRemoteAddress, 8);
            Terminal.print(F("Source endpoint: "));
            Terminal.println(packetSourceEndpoint, HEX);
            Terminal.print(F("Destinetion endpoint: "));
            Terminal.println(packetDestinationEndpoint, HEX);
            Terminal.print(F("Cluster Id: "));
            Terminal.println(packetClusterId, HEX);
            Terminal.print(F("Profile Id: "));
            Terminal.println(packetProfileId, HEX);
            Terminal.print(F("Payload: "));
            printAsHexa(packetPayload,packetLength - EXP_RX_PKT_HEADER_SIZE);
            
        }else if (packetFrameType == PACKET_AT_CMD_RESPONSE){
            Terminal.print(F("Command: "));
            printAsHexa(packetCommand, 2);
            Terminal.print(F("Status: "));
            Terminal.println(packetStatus,HEX);
            Terminal.print(F("Payload: "));
            printAsHexa(packetPayload,packetLength - AT_CMD_RESP_PKT_HEADER_SIZE);
            
        }else {
            if(SHOW_ERROR){ERROR;
                Terminal.println(F("XbeePacket::parse: Unrecognized packet type. Packet may be valid, library implementation is incomplete."));}
        }
    }
    
    void XbeePacket::clear(){
        packetLength = 0;
        packetFrameType = 0;
        memset(packetPayload, 0, MESSAGE_BUFFER_SIZE);
        memset(packetCommand, 0, 2);
        packetFrameId = 0;
        packetSourceEndpoint = 0;
        packetDestinationEndpoint = 0;
        packetClusterId = 0;
        packetProfileId = 0;
        packetStatus = 0;
        packetChecksum = 0;
    }
    
}
