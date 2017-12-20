#include "Bluetooth.h"
#include "../Generic.h"

namespace nmBluetooth {
    HorizonBluetooth::HorizonBluetooth()
    {
        memset(bluetoothSerial, 0, sizeof(bluetoothSerial));
    }
    
    bool HorizonBluetooth::setupBluetooth(void) {
        INFO;
        Terminal.print(F("Initializing Bluetooth... "));
        LCDprint(F("BT..."), LEFT, currentLine += LCD_LINE_GAP);
        char moduleResponse[AT_BUFFER_ZISE];
        memset(moduleResponse, 0, AT_BUFFER_ZISE);
        bool bluetoothInitialized = true;
        String bluetoothSerialStr;
        
        //Boot Bluetooth Module
        digitalWrite(POWER_BLUETOOTH, LOW);
        digitalWrite(BT_MODE, LOW);//(HC-05)
        digitalWrite(BT_RST, HIGH);//(HC-05)
        
        delay(3000);
        digitalWrite(POWER_BLUETOOTH, HIGH);
        delay(1000);
        digitalWrite(BT_MODE, HIGH);//(HC-05)
        delay(1000);
        //  digitalWrite(BT_MODE, LOW);//(HC-06)
        //  delay(1000);
        
        SerialBT.begin(BT_BAUD_RATE);
        if(BLUETOOTH == 2){
            bluetoothInitialized &= sendAtCommand(BT_MODE_TX_REMOTE, NULL);//(HM10)
            bluetoothInitialized &= sendAtCommand(BT_NOTIFY_ON, NULL);//(HM10)
            bluetoothInitialized &= sendAtCommand(BT_PWR_6_DBM, NULL);//(HM10)
            bluetoothInitialized &= sendAtCommand(BT_PIN_OFF, NULL);//(HM10)
        }
        bluetoothInitialized &= sendAtCommand(BT_MAC, moduleResponse);
        
        bluetoothSerialStr = parseSerial(moduleResponse);//(HC-05)
        
        if(bluetoothSerialStr.length()==0){
            bluetoothSerialStr = "BT_ERROR";
            bluetoothInitialized = false;
        }
        
        bluetoothInitialized &= sendAtCommand(BT_SET_NAME + bluetoothSerialStr, NULL);
        bluetoothSerialStr.getBytes(bluetoothSerial, bluetoothSerialStr.length()+1);
        if(bluetoothInitialized) {
            Terminal.println(F("OK!"));
            INFO;
            Terminal.print(F("Serial bluetooth: "));
            Terminal.println(bluetoothSerialStr);
            LCDprint(bluetoothSerial, RIGHT, currentLine);
        } else {
            Terminal.println(F("ERROR"));
            LCDprint(F("ERROR"), RIGHT, currentLine);
        }
        return bluetoothInitialized;
    }
    
    String HorizonBluetooth::parseSerial(char *serial){
        String bluetoothSerialStr;
        String NAP;
        String UAP;
        String LAP;
        if(BLUETOOTH == 1){
            bluetoothSerialStr = String(&serial[6]);//(HC-05)
        } else if (BLUETOOTH == 2){
            bluetoothSerialStr = String(&serial[8]);
        }
        bluetoothSerialStr.replace("\n","");
        bluetoothSerialStr.replace("\r","");
        bluetoothSerialStr.replace("OK","");
        
        for(int i = 0; i < bluetoothSerialStr.length() ; i++){
            if(bluetoothSerialStr.charAt(i) == BT_END){
                bluetoothSerialStr = bluetoothSerialStr.substring(0,i);
            }
        }
        
        if(bluetoothSerialStr.length()==0){
            return "";
        }

        NAP = bluetoothSerialStr.substring(0,bluetoothSerialStr.indexOf(":"));
        UAP = bluetoothSerialStr.substring(bluetoothSerialStr.indexOf(":")+1,bluetoothSerialStr.lastIndexOf(":"));
        LAP = bluetoothSerialStr.substring(bluetoothSerialStr.lastIndexOf(":")+1);
        
        while (NAP.length() < 4) {
            NAP = "0" + NAP;
        }
        while (UAP.length() < 2) {
            UAP = "0" + UAP;
        }
        while (LAP.length() < 6) {
            LAP = "0" + LAP;
        }
        
        return NAP+UAP+LAP;
    }
    
    bool HorizonBluetooth::sendAtCommand(String cmd, char * response) {
        if(SerialBT.available()){SerialBT.readString();}
        
        uint32_t timeCount = millis();
        bool responseReceived = false;
        int iterator = 0;
        char readByte;
        if(SHOW_DEBUG){DEBUG;
            Terminal.print(F("Bluetooth AT: "));
            Terminal.println(cmd);}
        if(BLUETOOTH == 1){
            SerialBT.print(cmd + BT_EOT);//(HC05)
        } else{
            SerialBT.print(cmd);
        }
        
        while(!SerialBT.available() && ((millis() - timeCount) < TIME_OUT_AT));
        while(SerialBT.available()) {
            readByte = SerialBT.read();
            if(response != NULL && iterator < AT_BUFFER_ZISE){
                response[iterator++] = readByte;
            }
            responseReceived = true;
            timeCount = millis();
            while(!SerialBT.available() && ((millis() - timeCount) < TIME_OUT_SERIAL));
        }
        if(SHOW_DEBUG){DEBUG;
            Terminal.print(F("Bluetooth AT response: "));
            Terminal.println(response);}
        return responseReceived;
    }
    
    int HorizonBluetooth::getBluetoothMessage(){
        uint32_t timeCount = millis();
        char localMessageBuffer[MESSAGE_BUFFER_SIZE];
        memset(localMessageBuffer, 0, MESSAGE_BUFFER_SIZE);
        
        int iterator = 0;
        int messageSize = 0;
        int readByte;
        int endIdentifier = LOCAL_IDENTIFIER;
        bool readRemoteMessage = false;
        int receivedMessageType = MSG_NONE;
        
        if(SerialBT.available()){
            if(!readRemoteMessage){
                readByte = SerialBT.read();
                if(readByte == MESSAGE_SEPARATOR) {
                    return 0;
                }
                if(readByte == REMOTE_IDENTIFIER) {
                    receivedMessageType = MSG_REMOTE;
                    endIdentifier = REMOTE_IDENTIFIER;
                    readRemoteMessage = true;
                }else {
                    receivedMessageType = MSG_LOCAL;
                    if(iterator < MESSAGE_BUFFER_SIZE){
                        localMessageBuffer[iterator++] = readByte;
                    }else {
                        if(SHOW_ERROR){ERROR;
                            Terminal.println(F("HorizonBluetooth::getBluetoothMessage: Bluetooth buffer overflow reading message"));}
                        return -1;
                    }
                }
            }
            timeCount = millis();
            while(!SerialBT.available() && ((millis() - timeCount) < TIME_OUT_SERIAL));
            
            while(SerialBT.available()) {
                readByte = SerialBT.read();
                if(readRemoteMessage){
                    if(iterator == 0){
                        messageSize = readByte;
                        if(messageSize > MESSAGE_BUFFER_SIZE){
                            if(SHOW_ERROR){ERROR;
                                Terminal.println(F("HorizonBluetooth::getBluetoothMessage: Insufficient memory allocated"));}
                            return -1;
                        }
                    } else if(readByte == endIdentifier){
                        if(iterator==messageSize+3 ){
                            readRemoteMessage = false;
                            break;
                        }
                    }
                }else if(readByte == LOCAL_IDENTIFIER || readByte == REMOTE_IDENTIFIER){
                    readRemoteMessage = false;
                    break;
                }
                
                if(iterator < MESSAGE_BUFFER_SIZE){
                    localMessageBuffer[iterator++] = readByte;
                }else {
                    if(SHOW_ERROR){ERROR;
                        Terminal.println(F("HorizonBluetooth::getBluetoothMessage: Bluetooth buffer overflow reading message"));}
                    return -1;
                }
                timeCount = millis();
                while(!SerialBT.available() && ((millis() - timeCount) < TIME_OUT_SERIAL));
            }
            
            if(receivedMessageType == MSG_LOCAL){
                bluetoothPacket.parse(receivedMessageType, localMessageBuffer, iterator);
            }else if(receivedMessageType == MSG_REMOTE){
                if(iterator!=messageSize+3){
                    if(SHOW_ERROR){ERROR;
                        Terminal.println(F("HorizonBluetooth::getBluetoothMessage: Bluetooth message corrupted: unmatched sizes"));
                        Terminal.print(F("Expected: "));
                        Terminal.println(messageSize);
                        Terminal.print(F("Received: "));
                        Terminal.println(iterator);}
                    return -1;
                }
                uint8_t packet[messageSize];
                memcpy(packet,&localMessageBuffer[1],messageSize);
                uint16_t crc16 = calcCRC(packet,messageSize);
                uint16_t btCrc16 = (localMessageBuffer[messageSize+1] << 8) | (localMessageBuffer[messageSize+2] & 0x00FF);
                if(crc16!=btCrc16){
                    if(SHOW_ERROR){ERROR;
                        Terminal.println("HorizonBluetooth::getBluetoothMessage: Bluetooth message corrupted: CRC is incorrect");}
                    return -1;
                }
                bluetoothPacket.parse(receivedMessageType, localMessageBuffer, iterator);
            }
        }else{
            return -1;
        }
        return 1;
    }

    bool HorizonBluetooth::sendMessage(){
        bool messageSent = false;
        if(bluetoothPacket.getSize()>MESSAGE_BUFFER_SIZE){
            if(SHOW_ERROR){ERROR;
                Terminal.println(F("HorizonBluetooth::sendMessage: Message size exceeds maximum size"));
                Terminal.print(F("Message size: "));
                Terminal.println(bluetoothPacket.getSize());
                Terminal.print(F("Maximum size: "));
                Terminal.println(MESSAGE_BUFFER_SIZE);}
            messageSent = false;
        }else {
            uint8_t packet[bluetoothPacket.getSize()];
            memcpy(packet,bluetoothPacket.getPayload(),bluetoothPacket.getSize());
            uint16_t crc16 = calcCRC(packet,bluetoothPacket.getSize());
            
            SerialBT.write(BT_END);
            SerialBT.write(char(bluetoothPacket.getSize()));
            SerialBT.write(char(bluetoothPacket.getCommand()));
            SerialBT.write(bluetoothPacket.getPayload(),bluetoothPacket.getSize()-1);
            SerialBT.write(char(crc16>>8));
            SerialBT.write(char(crc16 & 0x00FF));
            SerialBT.println(BT_END);
            messageSent = true;
        }
        return messageSent;
    }
    
    bool HorizonBluetooth::tryGetSerial(){
        if(SerialBT.available()){
            return true;
        }return false;
    }
    
    void HorizonBluetooth::newPacket(char command, char *payload, int size){
        bluetoothPacket.clear();
        bluetoothPacket.packetCommand = command;
        memset(bluetoothPacket.packetPayload, 0, MESSAGE_BUFFER_SIZE);
        memcpy(bluetoothPacket.packetPayload, payload, size);
        bluetoothPacket.packetSize = size + 1;
    }
    
    //PACKET
    void BluetoothPacket::setCommand(char command){
        packetCommand = command;
    }
    void BluetoothPacket::setSize(int size){
        packetSize = size;
    }
    bool BluetoothPacket::setPayload(char *payload, int size){
        if(size < MESSAGE_BUFFER_SIZE - 5){
            memcpy(packetPayload, payload, size);
            packetSize = size + 1;
            return true;
        }
        if(SHOW_ERROR){ERROR;
            Terminal.println(F("BluetoothPacket::setPayload: Invalid payload size"));}
        return false;
    }
    
    void BluetoothPacket::clear(){
        packetCommand = '0';
        packetSize = 0;
        memset(packetPayload, 0, MESSAGE_BUFFER_SIZE);
    }
    
    bool BluetoothPacket::parse(int type, char *data, int size){
        bool success = false;
        clear();
        
        if(type == MSG_LOCAL){
            packetCommand = CMD_LOCAL;
            memcpy(packetPayload, data, size);
            packetSize = size;
            success = true;
            
        }else if(type == MSG_REMOTE){
            packetSize = data[0] & 0x00FF;
            packetCommand = data[1];
            memcpy(packetPayload, &data[2], packetSize-1);
            success = true;
            
        }else if(type == MSG_NONE){
            
            success = true;
            
        }
        return success;
    }
    
    void BluetoothPacket::print(){
        Terminal.println(F("Bluetooth packet"));
        Terminal.print(F("Packet length: "));
        Terminal.println(packetSize);
        Terminal.print(F("Command: "));
        Terminal.println(packetCommand);
        Terminal.print(F("Payload: "));
        printAsHexa(packetPayload, packetSize -1);
    }
}
