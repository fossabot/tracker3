//
// horizon-tracker
//
// Description of the project
// Developed with [embedXcode](http://embedXcode.weebly.com)
//
// Author 		Eduardo Carvalho
// 				Strider
//
// Date			7/7/17 7:09 PM
// Version		<#version#>
//
// Copyright	© Eduardo Carvalho, 2017
// Licence		<#licence#>
//
// See         ReadMe.txt for references
//


// Core library for code-sense - IDE-based
#if defined(ENERGIA) // LaunchPad specific
#include "Energia.h"
#elif defined(TEENSYDUINO) // Teensy specific
#include "Arduino.h"
#elif defined(ESP8266) // ESP8266 specific
#include "Arduino.h"
#elif defined(ARDUINO) // Arduino 1.8 specific
#include "Arduino.h"
#else // error
#error Platform not defined
#endif // end IDE

// Set parameters
#define LCD_TIMEOUT_SEC 30
#define LCD_UPDATE_SEC 10


// Include application, user and local libraries
#include "Generic.h"
int currentLine;

// System flags with initial values
uint32_t lcdOnAt = 0;
uint32_t lcdOffAt = 0;
uint32_t lastReportAt = 0;
//uint32_t lastConnectionAttemptAt = 0;
uint32_t lastSignalLevelCheckAt = 0;
uint32_t offlineDataDisableAt = 0;


bool accInterruptDetected = false;

unsigned long logFileNumber = 0;
// Define structures and classes

MonitorData monitorData;


// Define variables and constants
nmGps::HorizonGPS                       horizonGPS;
nmAccelerometer::HorizonAccelerometer   horizonAccelerometer;
nmBluetooth::HorizonBluetooth           horizonBluetooth;
nmXbee::HorizonXbee                     horizonXbee;

char destinationAddress[16] = XBEE_DEST;

OrbBluetoothHeader orbHeader;

bool justStarted = true;
bool wasMoving = false;
bool bluetoothConnected = false;
bool enableLcdOnMovement = true;
bool enableTx = true;
bool enableOfflineData = true;
bool enableOfflineDataSend = true;
bool horizonConnected = true;
bool motionEnded;
bool motionStart;
int batteryLevel = 0;
bool gpsAntennaRemoved = false;
bool logSaved = true;

bool button = true;

bool xbeeAccess = false;
uint32_t requestAccessAt = 0;

uint8_t updateSignal = 0;
bool updateBtStatus = false;
byte localSystemStatus = 0x00;

uint32_t reportInterval = DEFAULT_REPORT_INTERVAL; // Send report every X miliseconds
uint32_t reportStoppedInterval = STOP_TRACK_INTERVAL;
uint32_t centralAliveAt = 0;
// Prototypes


// Utilities

char logText[128];     // Data that will be saved to LOG files

// Functions
void movementInterrupt(void) {
    accInterruptDetected = true;
}

// Add setup code
void setup()
{
    pinMode(LED, OUTPUT);
    pinMode(ACCELEROMETER_INTERRUPT_PIN, INPUT_PULLUP);
    pinMode(ONE_WIRE_BUS, INPUT);
    pinMode(POWER_BLUETOOTH, OUTPUT);
    pinMode(BT_MODE, OUTPUT);//(HC-05)
    pinMode(BT_RST, OUTPUT);//(HC-05)
    pinMode(POWER_XBEE, OUTPUT);
    delay(1000);
    
    Terminal.begin(TERMINAL_BAUD_RATE);
    Terminal.println();
    
    
    INFO;Terminal.print(F("Horizon Tracker - BUILD "));
    Terminal.println(VERSION);
    
    setupLCD();
    LCDprint("HORIZON TRACKER", CENTER, currentLine);
    LCDprintNumI(VERSION, CENTER, currentLine += LCD_LINE_GAP, NULL);
    
    if(GPS > 0){
        horizonGPS = nmGps::HorizonGPS();
        if(!horizonGPS.setupGPS()){
            FATAL_ERROR;
            Terminal.println("setup: Error initializing gps, check wiring and power source");
        }
    }else{
        INFO;
        Terminal.println(F("GPS disabled..."));
        LCDprint(F("GPS disabled..."), LEFT, currentLine += LCD_LINE_GAP_SMALL);
    }
    if(ACCELEROMETER > 0){
        horizonAccelerometer = nmAccelerometer::HorizonAccelerometer();
        if(horizonAccelerometer.setupAccelerometer()){
#ifdef STRIDER_BOARD
            attachInterrupt(digitalPinToInterrupt(ACCELEROMETER_INTERRUPT_PIN), movementInterrupt, FALLING);
#else
            attachPCINT(digitalPinToPCINT(ACCELEROMETER_INTERRUPT_PIN), movementInterrupt, FALLING);
#endif
        }
    }else{
        INFO;
        Terminal.println(F("Accelerometer disabled..."));
        LCDprint(F("Acceler. disabled..."), LEFT, currentLine += LCD_LINE_GAP);
    }
    
    if(SDCARD > 0){
        if(!nmSdCard::setupSDCard()){
            FATAL_ERROR;
            Terminal.println(F("setup: Error initializing sd card, check if it is connected."));
        }else{
            if(SHOW_DEBUG){DEBUG;
                Terminal.println(F("setup: Read file name from file"));}
            logFileNumber = 0;
            
//            nmSdCard::rmDir(LOG_DIR);
            if(!nmSdCard::exists(LOG_DIR)){
                if(SHOW_DEBUG){DEBUG;
                    Terminal.println(F("setup: create log directory"));}
                nmSdCard::mkDir(LOG_DIR);
            }
            
            if(nmSdCard::exists(STATUS_FILE)){
                char lastFileName[16];
                memset(lastFileName, 0, 16);
                nmSdCard::readLine(STATUS_FILE, lastFileName);
                logFileNumber = strtoul(lastFileName, NULL, HEX) + 1;
//                if(SHOW_DEBUG){DEBUG;
                    Terminal.print(F("setup: last file name: "));
                    Terminal.println(lastFileName);//}
            }else{
                if(SHOW_DEBUG){DEBUG;
                    Terminal.println(F("setup: write new status file"));}
                nmSdCard::writeSDline(STATUS_FILE, "00000000");
                if(SHOW_TODO){TODO;
                    Terminal.println(F("setup: chek if LOG dir is empty"));}
                //                if(nmSdCard::isEmpty(LOG_DIR)){
                //                    nmSdCard::writeSDline(STATUS_FILE, "00000000\r\n");
                //                }else{
                //                    if(SHOW_FATAL_ERROR){FATAL_ERROR; TODO;
                //                        Terminal.println(F("setup: sync old log files"));}
                //                }
            }
        }
    }else{
        INFO;
        Terminal.println(F("SD Card disabled..."));
        LCDprint(F("SD Card disabled..."), LEFT, currentLine += LCD_LINE_GAP);
    }
    
    if(BLUETOOTH > 0){
        horizonBluetooth = nmBluetooth::HorizonBluetooth();
        if(!horizonBluetooth.setupBluetooth()){
            FATAL_ERROR;
            Terminal.println("setup: Error initializing bluetooth, reseting device!");
            LCDprint(F("RESETING"), CENTER, currentLine += LCD_LINE_GAP);
            softwareReset();
        }
    }else{
        INFO;
        Terminal.println(F("Bluetooth disabled..."));
        LCDprint(F("Bluetooth disabled..."), LEFT, currentLine += LCD_LINE_GAP);
    }
    
    if(XBEE > 0){
        horizonXbee = nmXbee::HorizonXbee();
        if(!horizonXbee.setupXBee(destinationAddress, XBEE_MODE)){
            FATAL_ERROR;
            Terminal.println(F("setup: Error initializing xbee, reseting device!"));
            LCDprint(F("RESETING"), CENTER, currentLine += LCD_LINE_GAP);
            softwareReset();
        }
    }else{
        INFO;
        Terminal.println(F("XBee disabled...\r\n"));
        LCDprint("XBee disabled...", LEFT, currentLine += LCD_LINE_GAP);
    }
    
    justStarted = true;
    
    
    monitorData.update();
    
    delay(5000);
    LCDclearScreen();
    centralAliveAt = millis();
    LCD_OFF;
    switchLed();
    wdt_enable(WDTO_8S);
}

// Add loop code
void loop()
{
    wdt_reset();
    motionEnded = false;
    motionStart = false;
    
    if(justStarted){
        if(SHOW_DEBUG){DEBUG;
            Terminal.print(F("LOOP MEMORY: "));
            Terminal.println(FreeRam());
            delay(1000);
        }
    }
    
    if(button) button = digitalRead(ONE_WIRE_BUS);
    
    if (accInterruptDetected) {
        accInterruptDetected = false;
        horizonAccelerometer.getInterruptStatus();
    }
    
    if((enableLcdOnMovement && horizonAccelerometer.isMovementDetected(true)) || (!button)) {
        wdt_reset();
        if(!isLcdOn() && (timedOut(horizonAccelerometer.getLastStopAt(), MINIMUM_MOVEMENT_TIME) || (!button))) {
            LCD_ON;
            updateSignal = horizonXbee.getPowerLevel();
            updateBtStatus = bluetoothConnected;
            LCDdrawBitmap(5, 95, 25, 29, striderSmall); // Print Strider Logo // drawBitmap (x, y, sx, sy, data[, scale]);
            char destAdd[20];
            toHexaString(destAdd, horizonXbee.getDestinationAddress(), 8);
            localSystemStatus = monitorData.systemStatus;
            printLcdData(horizonXbee.getDeviceAddress(),
                         destAdd,
                         horizonBluetooth.getSerial(),
                         horizonXbee.getPowerLevel(),
                         bluetoothConnected,
                         nmSdCard::isOk());
        } else {
            resetLcdTimeout();
        }
        button = true;
    }else if(isLcdOn() && timedOut(lcdOnAt, LCD_TIMEOUT_SEC)) { // If LCD Screen On timeout then turn it off
        LCD_OFF;
    }
    
    if(isLcdOn()){
        wdt_reset();
        if(updateSignal != horizonXbee.getPowerLevel() || updateBtStatus != bluetoothConnected || localSystemStatus != monitorData.systemStatus){
            updateSignal = horizonXbee.getPowerLevel();
            updateLcdData(horizonXbee.getPowerLevel(), bluetoothConnected);
            localSystemStatus = monitorData.systemStatus;
        }
    }
    
    if(horizonXbee.analiseStoredPack()){
        horizonXbee.setPacketStored(false);
        analyseXbeePack();
    }
    
    if(horizonXbee.tryGetSerial()){
        wdt_reset();
        //if(SHOW_INFO){INFO;
        Terminal.println(F("Receiving Xbee packet"));//}
        if(horizonXbee.getXbeePacket()){
            analyseXbeePack();
        }
    }
    
    
    if(horizonBluetooth.tryGetSerial()){
        wdt_reset();
        bluetoothConnected = true;
        if(horizonBluetooth.getBluetoothMessage()>0){
            if(horizonBluetooth.bluetoothPacket.getCommand() == CMD_LOCAL){
                if(SHOW_TODO){TODO;
                    Terminal.println(F("loop: Implement decode local bluetooth message"));}
                if(SHOW_INFO){INFO;
                    Terminal.println(F("Receive bluetooth module message: should decode it."));}
                
                bluetoothConnected = false;
                
            }else if (horizonBluetooth.bluetoothPacket.getCommand() == CMD_PARAMETERS){
                char tmp[48];
                memset(tmp, 0, sizeof(tmp));
                char destAdd[16];
                toHexaString(destAdd, horizonXbee.getDestinationAddress(), 8);
                sprintf_P(tmp + strlen(tmp), PSTR("%s;%d;%s"),horizonXbee.getDeviceAddress(),horizonConnected,destAdd);
                horizonBluetooth.newPacket(CMD_PARAMETERS, tmp, strlen(tmp));
                horizonBluetooth.sendMessage();
            }else if (horizonBluetooth.bluetoothPacket.getCommand() == CMD_XBEE_ACCES){
                //if(SHOW_INFO){INFO;
                Terminal.println(F("Receive acces xbee message"));//}
                
                orbHeader.parse(horizonBluetooth.bluetoothPacket.getPayload());
                char tmp[30];
                memset(tmp, 0, sizeof(tmp));
                orbHeader.toArray(tmp);
                horizonBluetooth.newPacket(CMD_XBEE_ACCES, tmp, 30);
                horizonBluetooth.sendMessage();
                xbeeAccess = true;
                requestAccessAt = millis();
            }else if (horizonBluetooth.bluetoothPacket.getCommand() == CMD_XBEE_PAYLOAD){
                //if(SHOW_INFO){INFO;
                Terminal.println(F("Receive payload xbee message"));//}
                
                char tmp[MESSAGE_BUFFER_SIZE];
                memset(tmp, 0, MESSAGE_BUFFER_SIZE);
                memcpy(tmp, orbHeader.getPacketUUID(), 16);
                memcpy(&tmp[16], horizonBluetooth.bluetoothPacket.getPayload(), horizonBluetooth.bluetoothPacket.getSize()-1);
                horizonXbee.newPacket(PACKET_EXPLICIT_ADDRESS_CMD_FRAME, NULL, orbHeader.getPacketRemoteAddress(), 2, orbHeader.getPacketSourceEndpoint(), orbHeader.getPacketDestinationEndpoint(), orbHeader.getPacketClusterId(), orbHeader.getPacketProfileId(), tmp, horizonBluetooth.bluetoothPacket.getSize()+15);
                if(horizonXbee.sendPacket()){
                    memset(tmp, 0, MESSAGE_BUFFER_SIZE);
                    orbHeader.toArray(tmp);
                    horizonBluetooth.newPacket(CMD_ACK, tmp, 30);
                    horizonBluetooth.sendMessage();
                    //if(SHOW_INFO){INFO;
                    Terminal.println(F("ORB packet sent"));//}
                }else{
                    memset(tmp, 0, MESSAGE_BUFFER_SIZE);
                    orbHeader.toArray(tmp);
                    horizonBluetooth.newPacket(CMD_NACK, tmp, 30);
                    horizonBluetooth.sendMessage();
                    // if(SHOW_INFO){INFO;
                    Terminal.println(F("Failed to send ORB packet"));//}
                }
                xbeeAccess = false;
                
            }else if (horizonBluetooth.bluetoothPacket.getCommand() == CMD_GPS_UPDATE){
                if(SHOW_TODO){TODO;
                    Terminal.println(F("loop: Implement gps update"));}
                if(SHOW_INFO){INFO;
                    Terminal.println(F("Receive GPS update message: should decode it."));}
                
            }
        }
    }
    
    if(xbeeAccess && timedOut(requestAccessAt, 5)){
        xbeeAccess = false;
        char tmp[30];
        memset(tmp, 0, sizeof(tmp));
        orbHeader.toArray(tmp);
        horizonBluetooth.newPacket(CMD_NACK, tmp, 30);
        horizonBluetooth.sendMessage();
    }
    
    if(horizonGPS.tryGetSerial()){
        wdt_reset();
        if(horizonGPS.encode()){
            //            if(horizonGPS.isDataUpdated()){
            if(horizonAccelerometer.isStopped() && wasMoving) {
                motionEnded = true;
            }
            if((horizonAccelerometer.isMoving() && !wasMoving) || justStarted) {
                motionStart = true;
            }
            
            wasMoving = horizonAccelerometer.isMoving();
            
            monitorData.update();
            if(justStarted) {
                justStarted = false;
            }
            
            memset(logText, 0, sizeof(logText));
            monitorData.toCharArray(logText);
            
            //            memcpy(logText, "FAKE LOG\r\n", 10);
            if(SHOW_LOG){LOG;
                Terminal.print(logText);}
            
            if(bluetoothConnected){
                horizonBluetooth.newPacket(CMD_MONITOR, logText, strlen(logText));
                horizonBluetooth.sendMessage();
            }
            
            
            //inserir condiçoes:
            // horizon connected
            // gpsAntennaRemoved
            // time
            bool reportData = !horizonAccelerometer.isMoving() && timedOut(lastReportAt, reportStoppedInterval) || horizonAccelerometer.isMoving() || motionStart || motionEnded;
            
            if(reportData){
                lastReportAt = millis();
                if(enableTx && horizonConnected){
                    if(horizonXbee.isOnline()){
                        //if(SHOW_INFO){INFO;
                        Terminal.println(F("Sending monitor data"));//}
                        horizonXbee.newPacket(PACKET_EXPLICIT_ADDRESS_CMD_FRAME, NULL, horizonXbee.getDestinationAddress(), 2, SOURCE_ENDPOINT_DEFAULT, 0, 0, 0, logText, strlen(logText));
                        if(!horizonXbee.sendPacket()){
                            if(SHOW_INFO){INFO;
                                Terminal.println(F("Failed to send data: saving data"));}
                            if(enableOfflineData){
                                logSaved = saveLog(logText, logFileNumber);
                                logFileNumber++;
                            }
                        }else {
                            Terminal.println(F("Packet sent"));
                        }
                        
                    }else {
                        //if(SHOW_INFO){INFO;
                        Terminal.println(F("Offline: saving data"));//}
                        if(enableOfflineData){
                            logSaved = saveLog(logText, logFileNumber);
                            logFileNumber++;
                        }
                        
                    }
                }else {
                    if(SHOW_INFO){INFO;
                        Terminal.println(F("TX disable: saving data"));}
                    if(enableOfflineData){
                        logSaved = saveLog(logText, logFileNumber);
                        logFileNumber++;
                    }
                }
            }
        }
    }
    
    if(enableTx && enableOfflineData && enableOfflineDataSend && horizonConnected){
        wdt_reset();
        if(horizonXbee.isOnline()){
            char offlineLog[128];     // Data that will be saved to LOG files
            char fileName[16];     // Data that will be saved to LOG files
            memset(fileName, 0, sizeof(fileName));
            memset(offlineLog, 0, sizeof(offlineLog));
            if(!nmSdCard::isEmpty(LOG_DIR)){
                
                wdt_disable();
                if(nmSdCard::readNextLog(fileName, offlineLog)){
                    //if(SHOW_INFO){INFO;
                    Terminal.println(F("Sending offline data"));//}
                    
                    horizonXbee.newPacket(PACKET_EXPLICIT_ADDRESS_CMD_FRAME, NULL, horizonXbee.getDestinationAddress(), 2, SOURCE_ENDPOINT_DEFAULT, CMD_OFFLINE_DATA, 0, 0, offlineLog, strlen(offlineLog));
                    if(horizonXbee.sendPacket()){
                        if(SHOW_INFO){INFO;
                            Terminal.println(F("loop: Offline monitor data sent, removing file."));}
                        char filePath[24];
                        memset(filePath, 0, sizeof(filePath));
                        sprintf_P(filePath + strlen(filePath), PSTR("%s/%s"),LOG_DIR,fileName);
                        if(!nmSdCard::remove(filePath)){
                            if(SHOW_ERROR){ERROR;
                                Terminal.print("Could not remove file: ");
                                Terminal.println(filePath);}
                        }
                    }else{
                        if(SHOW_INFO){INFO;
                            Terminal.println(F("loop: Offline monitor data not sent, keep file."));}
                    }
                }
                wdt_enable(WDTO_8S);
            }else{
                if(SHOW_INFO){INFO;
                    Terminal.println(F("loop: No offline data to send"));}
            }
        }
    }
    
    if(enableOfflineData && enableOfflineDataSend){
        if(nmSdCard::exists(PRIMARY_FILE)){
            wdt_disable();
            copyFileQueue(PRIMARY_FILE);
            wdt_enable(WDTO_8S);
        }else if(nmSdCard::exists(SECONDARY_FILE)){
            wdt_disable();
            copyFileQueue(SECONDARY_FILE);
            wdt_enable(WDTO_8S);
        }
        if(!nmSdCard::isEmpty(LOG_DIR)){
            char offlineLog[128];     // Data that will be saved to LOG files
            char fileName[16];     // Data that will be saved to LOG files
            memset(fileName, 0, sizeof(fileName));
            memset(offlineLog, 0, sizeof(offlineLog));
            if(!nmSdCard::readNextLog(fileName, offlineLog)){
                char filePath[24];
                memset(filePath, 0, sizeof(filePath));
                sprintf_P(filePath + strlen(filePath), PSTR("%s/%s"),LOG_DIR,fileName);
                wdt_disable();
                copyFileQueue(filePath);
                wdt_enable(WDTO_8S);
            }
        }
    }
    
    if(timedOut(lastSignalLevelCheckAt, 15)){
        lastSignalLevelCheckAt = millis();
        horizonXbee.readPowerLevel();
        nmSdCard::isSDPresent();
    }
    
    if(timedOut(centralAliveAt, 60*3)){
        horizonConnected = false;
    }
    
    /*  ---------------------------ATENÇÃO--------------------------
     VESÃO PARA COMUNICADOR MANUAL DEVE COMENTAR AS LINHAS ABAIXO
     ------------------------------------------------------------*/
    
    if(enableOfflineData && !enableOfflineDataSend && timedOut(offlineDataDisableAt, 60*1)){
        enableOfflineDataSend = true;
    }
    
}
