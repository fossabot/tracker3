#define SW_TRACKER_VERSION 20

// TIPO,VERSAO,SERIAL,LATITUDE,LONGITUDE,GPS_STATUS,DATE,TIME,SPEED,COURSE,ALTITUDE,SATS,HDOP,ACCELEROMETER,SIGNAL_LEVEL
// T,0002,88C2551E351C,-20.914515,-44.878788,VALID,25112016,173146,0.00,42,1069,11,0.73,MOVE,85

//#define MOCKDATA // ATTENTION !!!!!!!!!!!!!!!! DISABLE THIS BEFORE DEPLOYING
//#define DEBUG // ATTENTION !!!!!!!!!!!!!!!! DISABLE THIS BEFORE DEPLOYING
//#define DEBUG_BT // ATTENTION !!!!!!!!!!!!!!!! DISABLE THIS BEFORE DEPLOYING
//#define DEBUG_BT_INOUT // ATTENTION !!!!!!!!!!!!!!!! DISABLE THIS BEFORE DEPLOYING
//#define DEBUG_EEPROM // ATTENTION !!!!!!!!!!!!!!!! DISABLE THIS BEFORE DEPLOYING
//#define MEMORY_DEBUG // ATTENTION !!!!!!!!!!!!!!!! DISABLE THIS BEFORE DEPLOYING
//#define DEBUG_RECEIVED_MESSAGES // ATTENTION !!!!!!!!!!!!!!!! DISABLE THIS BEFORE DEPLOYING
//#define DEBUG_XBEE // ATTENTION !!!!!!!!!!!!!!!! DISABLE THIS BEFORE DEPLOYING
//#define TERMINAL_LOG
//#define DEBUG_ORB

// ATTENTION !!!!!!!!!!!!!!!! ENABLE ALL THIS BEFORE DEPLOYING
#define LCD 1
#define GPS 2 // (1 = GPS uBlox NEO-M8 / 2 = GPS NEO-6M / RoyalTek REB-5216)
#define BLUETOOTH 0 //(0 = HC05 / 1 = HM10 / 2 = HC06)
#define XBEE 2 // 1 = Transparent mode / 2 = API Mode
#define ACCELEROMETER 1
#define SDCARD 1
#define SHOW_LOGO
//#define WRITE_EEPROM
//#define CLEAR_EEPROM
//#define ORB
//#define DISCOVER_DEVICES

#define STRIDER_BOARD
//#define PROGRAMMABLE_XBEE
//#define SET_RTC

// https://www.u-blox.com/sites/default/files/products/documents/PowerManagement_AppNote_(UBX-13005162).pdf
// STRIDER FIXED STATION ADDRESS 0013A20040A7DBB4
//#define XBEE_DEST "0013A20040C63583" // MODEM USB
//#define XBEE_DEST "0013A20040D9EE16" // MODEM RS-232
//#define XBEE_DEST "0013A20040AC6F89" // Programmable UFL
//#define XBEE_DEST "0013A20040B3CBFF" // Programmable Wire
//#define XBEE_DEST "000000000000FFFF"


#define XBEE_DEST "0013A20041072659" // central VELOSO
//#define XBEE_DEST "0013A200415B7AC5" // central Agrisan
//#define XBEE_DEST "0013A200415B7A98" // central TSUGE 01
//#define XBEE_DEST "0013A200415B7AC1" // central TSUGE 02
//#define XBEE_DEST "0013A200415215CF"// FOCA OLAM
//#define XBEE_DEST "0013A20041571B18" // LOCAL TEST
//#define XBEE_DEST "0013A20040A7DBB4" // LOCAL TEST
//#define XBEE_DEST "0013A200410726A4" // LOCAL TEST

//#define XBEE_TRANSFER_DEST "0013A20040C63583" // MODEM USB
//#define XBEE_TRANSFER_DEST "0013A20040D9EE16" // MODEM RS-232
//#define XBEE_TRANSFER_DEST "0013A20040AC6F89" // Programmable UFL
//#define XBEE_TRANSFER_DEST "0013A20040B3CBFF" // Programmable Wire


#define XBEE_TRANSFER_DEST "0013A20041072659" // central VELOSO
//#define XBEE_TRANSFER_DEST "0013A200415B7AC5" // central Agrisan
//#define XBEE_TRANSFER_DEST "0013A200415B7A98" // central TSUGE 01
//#define XBEE_TRANSFER_DEST "0013A200415B7AC1" // central TSUGE 02
//#define XBEE_TRANSFER_DEST "0013A200415215CF"// FOCA OLAM
//#define XBEE_TRANSFER_DEST "0013A20041571B18" // LOCAL TEST
//#define XBEE_TRANSFER_DEST "0013A20040A7DBB4" // LOCAL TEST
//#define XBEE_TRANSFER_DEST "0013A200410726A4" // LOCAL TEST

//#ifdef STRIDER_BOARD
  #define GPS_BAUD_RATE 9600
//#else
//  #define GPS_BAUD_RATE 4800//38400
//#endif

#define ORIENTATION LANDSCAPE
#define FW_FILENAME "FIRMWARE.BIN"

#define OFFLINE_FILE1 "OFFLINE1.TXT"
#define OFFLINE_FILE2 "OFFLINE2.TXT"

#include "Generic.h"

const PROGMEM uint32_t VERSION = SW_TRACKER_VERSION;

String xbeeTransferDestination = XBEE_TRANSFER_DEST;
String xbeeDestination = XBEE_DEST;
String xbeeDeviceSerial = "NO_XBEE";

char xbeeDeviceSerialL[16];// = "NO_XBEE";

String btSerial = "NO_BT";

/**************************************************************/
// Default Settings
/**************************************************************/
uint32_t connectionRetryIntervalSeconds = 60; // Time to wait before retry connecting
uint32_t reportInterval = 3000; // Send report every X miliseconds
uint32_t reportStoppedInterval = STOP_TRACK_INTERVAL;
uint32_t lcdOnTimeoutSeconds = 30; // Turn LCD OFF after X seconds its being turned on, unless there's movement

bool enableLcdOnMovement = true;
bool enableLcdAutomatically = false; // Turn ON display in LCD every "lcdOnPeriodSeconds"
uint32_t lcdOnIntervalSeconds = 30; // Turn LCD ON every X seconds (if enableLcdAutomatically is TRUE)

bool writeSd = true;
uint32_t SDWriteIntervalSeconds = 30;

uint8_t mode = 0; // 0 = Autonomous <-> 1 = 485 Bypass
bool txEnabled = true;

uint8_t stressMode = 0;

/**************************************************************/
// System flags with initial values
uint32_t lcdOnAt = 0;
uint32_t lcdOffAt = 0;
uint32_t lastReportAt = 0;
uint32_t lastSdWriteAt = 0;
uint32_t lastMovementAt = 0;
uint32_t lastStopAt = 0;
uint32_t lastConnectionAttemptAt = 0;

bool lcdJustTurnedOn = false;
uint8_t xbeePowerLevel = 0;

bool sdCardError = true; // Initial value for proper card detection
bool interruptDetected = false;
bool movementDetected = false;
bool zeroMovementDetected = false;
bool rs485Error = false;
bool justStarted = true;
bool firstLocation = true;
bool gpsError = false;
bool gpsValid = false;
bool lastGpsPositionValid = false;
bool currentGpsPositionValid = false;
bool wasMoving = false;
bool offlineMode = false;
bool bluetoothConnected = false;
bool onlineMock = false;

bool gpsPositionAcquired = false;
bool gpsPositionLost = false;

double lastValidLat = 0;
double lastValidLng = 0;

char currentDateTime[20] = "01/01/2016 00:00:00";
char fileName[15];

int currentLine = INITIAL_LINE;

byte rx_buffer[XBEE_RX_BUFFER];
char tx_buffer[XBEE_TX_BUFFER];
char response_buffer[RESPONSE_BUFFER_SIZE];

char logText[100];     // Data that will be saved to LOG files
char logFileName[15];

String btGPS = "0.000000,0.000000";
String btSpeed = "0.00";
bool btValidGPS = false;

uint32_t lastBtMsg = 0;

// GLOBAL BT VARIABLES
int btMsgType;
char btCmd;
char btPayloadBuffer[256];
uint8_t btPayloadBufferSize;
char btMessageId[16];
char btMessageRequest[31];

uint32_t btAccessRequestTime = 0;
uint32_t btWaitPayloadTimeOut = 10;

//CHAT
bool remoteAccessRequest = false;
unsigned char remoteDestAddres[8];
unsigned char remoteSendPoint;
unsigned char remoteDendPoint;
unsigned char remoteClusterId[2];
unsigned char remoteProfileId[2];
unsigned char receivedSendPoint;
String receivedHeader;
unsigned int receivedDataLen;
tmElements_t tm;

String defaultConfig;
char defaultConfigByte[33];
uint16_t crcDefaultConfig;

bool button = true;

double gpsLat = 0;
double gpsLng = 0;
uint8_t gpsDay = 1;
uint8_t gpsMonth = 1;
uint16_t gpsYear = 1984;
uint8_t gpsHour = 0;
uint8_t gpsMinute = 0;
uint8_t gpsSecond = 0;
double gpsSpeed = 0.0;
double gpsCourse = 0.0;
double gpsAltitude = 0.0;  
uint32_t gpsSatellites = 0;
double gpsHdop = 0.0;
bool gpsLocationUpdated = false;
bool gpsLocationValid = false;


  bool motionStart = false;
  bool motionEnded = false;
   
void setup() {
  delay(1000);
    
  Terminal.begin(TERMINAL_BAUD_RATE); 
  Terminal.println();

  // Load settings from EEPROM (write default if empty)
  if(EEPROM.read(0x340) == 0xFF) {
    Terminal.println("\r\nEEPROM EMPTY, WRITING DEFAULT DATA...\r\n");
    EEPROM.write(0x340, onlineMock);
  }
  onlineMock = EEPROM.read(0x340);

  #ifdef CLEAR_EEPROM
    EepromUtil::eeprom_erase_all();
    delay(100);
  #endif

  #ifdef WRITE_EEPROM
    Terminal.println("Read EEPROM default config");
    EepromUtil::eeprom_read_string(0x300, defaultConfigByte, 33);
    defaultConfig = String(defaultConfigByte);
    crcDefaultConfig = EEPROM.read(0x320)*256 + EEPROM.read(0x321);
    delay(100);
  
    #ifdef DEBUG_EEPROM
      Terminal.print("CRC calculated for readed config: ");
      Terminal.println(calcCRC(defaultConfig.c_str(),defaultConfig.length()));
      Terminal.print("CRC readed: ");
      Terminal.println(crcDefaultConfig);
      EEPROM.write(0x320, 0xFF);
    #endif
     
    if(crcDefaultConfig == calcCRC(defaultConfig.c_str(),defaultConfig.length())){
      Terminal.println("Default config correct");
      xbeeDestination = defaultConfig.substring(0,16);
      xbeeTransferDestination = defaultConfig.substring(16,32);
    }else{
      Terminal.println("ERROR: Default config corrupted");
      defaultConfig = String(xbeeDestination + xbeeTransferDestination); 
      crcDefaultConfig = calcCRC(defaultConfig.c_str(),defaultConfig.length());
      EepromUtil::eeprom_write_string(0x300, defaultConfig.c_str());
      delay(100);
      EEPROM.write(0x320, crcDefaultConfig>>8);
      EEPROM.write(0x321, crcDefaultConfig&0x00ff);
    }
  #endif

  pinMode(LED, OUTPUT);
  pinMode(ACCELEROMETER_INTERRUPT_PIN, INPUT_PULLUP);
  pinMode(ONE_WIRE_BUS, INPUT);
  
  #ifdef STRIDER_BOARD
    attachInterrupt(digitalPinToInterrupt(ACCELEROMETER_INTERRUPT_PIN), movementInterrupt, FALLING);
  #else
    attachPCINT(digitalPinToPCINT(ACCELEROMETER_INTERRUPT_PIN), movementInterrupt, FALLING);
  #endif

//CONVERT DATE TIME TO LONG
//tm.Second = s; 
//tm.Minute = m; 
//tm.Hour = h; 
//tm.Wday = 6;   // day of week, sunday is day 1
//tm.Day = D;
//tm.Month = M; 
//tm.Year = Y-1970; 
//Terminal.println("TIME-STAMP");
//timestamp = makeTime(tm);
//Terminal.println(timestamp);
//Terminal.println(timestamp,HEX);
//
//
//Terminal.println(timestamp*1000);
//Terminal.println(timestamp*1000,HEX);

  setupLCD();

  Terminal.print(F("\r\nHorizon Tracker - BUILD "));
  Terminal.println(VERSION);
  LCDprint("HORIZON TRACKER", CENTER, currentLine);
  LCDprintNumI(VERSION, CENTER, currentLine += LCD_LINE_GAP, NULL);

  setupGPS();
//    while(1){
//      while(SerialGPS.available() > 0) 
//        Terminal.print(char(SerialGPS.read()));
//      }
  setupAccelerometer();
  setupSDCard();
  setupBluetooth();
  setupXBee();
  //LCDclearScreen();

  sprintf_P(xbeeDeviceSerialL + strlen(xbeeDeviceSerialL),PSTR("%s"), xbeeDeviceSerial.c_str());

  #ifdef DISCOVER_DEVICES
    discoverXBee();
    delay(5000);
  #endif
  
  delay(5000);
  
  LCDclearScreen();
  LED_OFF; 

  justStarted = true; // Flag to inform that we should write to SD initialize log

  LCD_OFF;
  
}

/*---------------------------------------------------------------------------*/
/**
 * Main loop
 */
/*---------------------------------------------------------------------------*/
void loop() {

//  #ifdef DEBUG
//    Terminal.println(F("LOOP..."));
//  #endif

  /*
   * Se posição não for enviada com sucesso (API Frame mode), ela deve ser salva em um arquivo temporário "RETRY.TXT"
   * que deverá ser lido quando a conexão for reestabelecida, (devemos enviar um comando de teste de conexão antes e ver se o mesmo foi recebido na outra ponta)
   * caso positivo, devemos ler o arquivo RETRY.TXT e reenviar todas as linhas (confirmando o recebimento), em caso de falha os pacotes agora deverão ser salvos em um
   * arquivo com o nome derivado do millis() e no final o RETRY.TXT devera ser apagado (sera recriado assim que uma posição nova falhar). E esse ciclo se repete constantemente
   * até que todas as posições que falharam sejam enviadas com sucesso.
   * E independente de erro, todas as posições são salvas em arquivos de logs diários 
   */

  currentLine = INITIAL_LINE;
  bool validPacket = false;
  bool moved = false;

  bool gpsUpdated = false;
  
  gpsPositionAcquired = false;
  gpsPositionLost = false;

  motionStart = false;
  motionEnded = false;

  if (interruptDetected) {
    interruptDetected = false;
    getInterruptStatus();
  }

  if(button) button = digitalRead(ONE_WIRE_BUS);

  
  #ifdef ORB
    if(!button){
      button = true;
      LCD_ON;
      lcdJustTurnedOn = true;
      lcd.drawBitmap(5, 95, 25, 29, striderSmall); // Print Strider Logo // drawBitmap (x, y, sx, sy, data[, scale]);
    } else if(isLcdOn() && timedOut(lcdOnAt, lcdOnTimeoutSeconds)) { // If LCD Screen On timeout then turn it off
      LCD_OFF;
    }
    printLcdData();
  #else
  // If movement is detected, we should turn on LCD till device stops again
    if(isMovementDetected(true)) {
      if(enableLcdOnMovement) {
        if(!isLcdOn() && timedOut(lastStopAt, MINIMUM_MOVEMENT_TIME)) {
          LCD_ON;
          lcdJustTurnedOn = true;
          lcd.drawBitmap(5, 95, 25, 29, striderSmall); // Print Strider Logo // drawBitmap (x, y, sx, sy, data[, scale]);
        } else {
          resetLcdTimeout();
        }
      }
  
    } else if(isLcdOn() && timedOut(lcdOnAt, lcdOnTimeoutSeconds)) { // If LCD Screen On timeout then turn it off
      LCD_OFF;
    }
    printLcdData();
  #endif


  // Read serial data if available
  int rx_count = readSerialDataIfAvailable();
  if(rx_count) {
    memset(response_buffer, 0, sizeof(response_buffer));
    #if XBEE == 1
      memcpy(response_buffer, rx_buffer, rx_count);
    #elif XBEE == 2
      analyseApiPacket(); // if RX packet, copy valid data to "response_buffer"
    #endif
    validPacket = analyseReceivedData();
  }


  // BLUETOOTH CONNECTION ANALYSIS
  while(1){ 
    if(SerialBT.available()){
      if(!getBluetoothMessage()) Terminal.println("ERROR WHILE READIND BLUETOOTH MESSAGE");
      else{
        if(btPayloadBuffer[0]==0xFE) {
          bluetoothConnected = true;
          break;
        }
        if(btMsgType == 0){
          break;
        }else {
          if(btMsgType == 1){
            int bt_cmd;
            bt_cmd = decodeBluetoothMessage();
            if(bt_cmd < 0) bluetoothConnected = false;
            if(bt_cmd == 2) bluetoothConnected = false;
            //if(bt_cmd == 1) bluetoothConnected = true;
          }else {
            #ifdef DEBUG_ORB
              Terminal.println("\n----------FROM BLUETOOTH----------");
              for(int i = 0; i<= btPayloadBufferSize ; i++){
                Terminal.print(char(btPayloadBuffer[i]));
              }Terminal.println();
            #endif
            bluetoothConnected = true;
            if(btMsgType == 2){
              switch (btCmd) {
                case 'G':
                  btValidGPS = true;
                  break;
                case 'X':
                  Terminal.println("XBee Access request");
                  if(remoteAccessRequest){
                    btMessageRequest[0] = 'N';
                    remoteAccessRequest = false;
                    sendRemoteMessage(btMessageRequest,31);
                    Terminal.println("ACESSO NEGADO POR EXCESSO DE REQUEST");
                    break;
                  }
                  memcpy(remoteDestAddres,&btPayloadBuffer[0],8);
                  remoteSendPoint=btPayloadBuffer[8];
                  remoteDendPoint=btPayloadBuffer[9];
                  memcpy(remoteClusterId,&btPayloadBuffer[10],2);
                  memcpy(remoteProfileId,&btPayloadBuffer[12],2);
                  
                  memcpy(&btMessageRequest[1],btPayloadBuffer,sizeof(btMessageRequest));
                  memcpy(btMessageId,&btPayloadBuffer[14],sizeof(btMessageId));
                  

                  btMessageRequest[0] = 'X';
                  remoteAccessRequest = true;
                    
                  sendRemoteMessage(btMessageRequest,btPayloadBufferSize);
//                  Terminal.println("XBee CONFIRMATION SENT");
                  
                  btAccessRequestTime = millis();

                  #ifdef DEBUG_ORB
                    Terminal.println("\n----------TO BLUETOOTH ACCESS----------");
                    for(int i = 0; i<= btPayloadBufferSize ; i++){
                      Terminal.print(char(btMessageRequest[i]));
                    }Terminal.println();
                  #endif

                  break;
                case 'M':
                  if(remoteAccessRequest){
                    Terminal.println("XBee payload received");
                    char tmpPayload[256];
                    memset(tmpPayload, 0, sizeof(tmpPayload));
                    memcpy(tmpPayload,btMessageId,sizeof(btMessageId));
                    memcpy(&tmpPayload[16],btPayloadBuffer,btPayloadBufferSize);
//                    Terminal.println("SENT PAYLOAD"); 
//                    for(int c = 0 ; c < btPayloadBufferSize+15; c++){
//                      Terminal.print(tmpPayload[c],HEX);
//                      Terminal.print(" ");
//                     }
//                     Terminal.println();
//                     Terminal.println(btPayloadBufferSize);
//                     Terminal.println(sizeof(tmpPayload));
                     
                    if(sendXbeeData(tmpPayload, false, btPayloadBufferSize+15)){
                      #ifdef DEBUG_ORB
                        Terminal.println("MESSAGE SENT TO XBEE");
                      #endif
                      btMessageRequest[0] = 'A';
                      sendRemoteMessage(btMessageRequest, sizeof(btMessageRequest));

                      #ifdef DEBUG_ORB
                        Terminal.println("\n----------TO BLUETOOTH ACK----------");
                        for(int i = 0 ; i<= btPayloadBufferSize ; i++){
                          Terminal.print(char(btMessageRequest[i]));
                        }Terminal.println();
                      #endif
                    }
                    else{ 
                      #ifdef DEBUG_ORB
                        Terminal.println("MESSAGE NOT SENT TO XBEE");
                      #endif
                    }
                    
                    remoteAccessRequest = false;
                    
                  } else {
                    //Notify access not guaranteed
                    btMessageRequest[0] = 'N';
                    remoteAccessRequest = false;
                    sendRemoteMessage(btMessageRequest,31);
                    Terminal.println("ACESSO NEGADO POR FALTA DE REQUEST");
//                    sendRemoteMessage("NAccess Denied");
                  }
                  break;
                case 'P':
//                  Terminal.println("Get Status");
                  char tmpPayload[256];
                  memset(tmpPayload, 0, sizeof(tmpPayload));
                  //sprintf_P(tmpPayload + strlen(tmpPayload), PSTR("P%s;%d\0"),xbeeDeviceSerialL.c_str(),!offlineMode);
                  tmpPayload[0]='P';
                  memcpy(&tmpPayload[1],xbeeDeviceSerialL,17);
                  tmpPayload[17]=';';
                  tmpPayload[18]=(offlineMode? '0' : '1');
                  
//                  Terminal.println(tmpPayload);
                  sendRemoteMessage(tmpPayload,strlen(tmpPayload));
                  memset(tmpPayload, 0, sizeof(tmpPayload));
                  break;
                
                default: 
                  Terminal.println("Invalid Command");
                  sendRemoteMessage("NOK");
                  // if nothing else matches, do the default
                  // default is optional
                break;
              }
            }
          }
        }
      }
      btClear();
    }
    if(!remoteAccessRequest){
      break; 
    }
    else{
      rx_count = readSerialDataIfAvailable();
      if(rx_count) {
        Terminal.println("READ INCOMING PACKET WHILE WAIT");
        memset(response_buffer, 0, sizeof(response_buffer));
        #if XBEE == 1
          memcpy(response_buffer, rx_buffer, rx_count);
        #elif XBEE == 2
          analyseApiPacket(); // if RX packet, copy valid data to "response_buffer"
        #endif
        validPacket = analyseReceivedData();
      }
      
      if(timedOut(btAccessRequestTime, btWaitPayloadTimeOut)){
        Terminal.println("TIME OUT");
        btAccessRequestTime = 0;
        // PUT TIME OUT HERE
        
        btMessageRequest[0] = 'N';
        sendRemoteMessage(btMessageRequest,31);
                    
        Terminal.println("ACESSO NEGADO POR TIME OUT");
        remoteAccessRequest = false;
        break; 
      }
    }
  }
  /***************/
// GPS UPDATE CODE
#if GPS > 0

  while(SerialGPS.available() > 0) {
    //Terminal.println("GOT GPS DATA");
    if(gps.encode(SerialGPS.read())) {
      gpsUpdated = analyseGPSData();
      if(gpsUpdated) {
       
        #ifdef DEBUG
          Terminal.print(F("GPS Updated..."));
          Terminal.println(currentGpsPositionValid ? F(" GPS FIX") : F(" NO GPS SIGNAL"));
        #endif

        lastGpsPositionValid = currentGpsPositionValid;
        
        break; // If GPS was updated and it was a valid packet, break the loop so we can analyze it.
      }
    }
  }
    
#endif // GPS
  /***************/
//
//  if(isStopped() && wasMoving) {
//    digitalWrite(POWER_GPS, LOW);
//    Terminal.println(F("GPS OFF"));
//  }
//  
//  if(isMoving() && !wasMoving) {
//    digitalWrite(POWER_GPS, HIGH);
//    Terminal.println(F("GPS ON"));
//  }
  
  if(gpsUpdated) {

    /*********************/
    #ifdef ACCELEROMETER

      if(isStopped() && wasMoving) {
        motionEnded = true;
        //DESCOMENTAR
        sendUBX(UBX_GNSS_DISABLE, sizeof(UBX_GNSS_DISABLE)); // Disable GNSS
        sendUBX(UBX_CFG_PM2_PSMCT____, sizeof(UBX_CFG_PM2_PSMCT____)); // Power save mode -> cyclic
        sendUBX(UBX_CFG_RXM_PWR_SAVE, sizeof(UBX_CFG_RXM_PWR_SAVE));
        sendUBX(UBX_CFG_RATE_10_SEC, sizeof(UBX_CFG_RATE_10_SEC)); // Set 10 seconds update rate

        #ifdef DEBUG
          Terminal.println(F("Motion ended... Entering GPS power save mode..."));
        #endif
      }
      
      if((isMoving() && !wasMoving) || justStarted) {
        motionStart = true;
        //DESCOMENTAR
        sendUBX(UBX_GNSS_ENABLED, sizeof(UBX_GNSS_ENABLED)); // Enable GNSS
        sendUBX(UBX_CFG_RXM_NORMAL, sizeof(UBX_CFG_RXM_NORMAL)); // DISABLE GPS POWER SAVE MODE
        sendUBX(UBX_CFG_RATE_10_SEC, sizeof(UBX_CFG_RATE_10_SEC)); // Set 1 second update rate
        
        #ifdef DEBUG
          Terminal.println(F("Motion started... Exiting GPS power save mode..."));
        #endif
      }

      wasMoving = isMoving();
    
      //LCDprint(accError ? "ACCELEROMETER ERROR" : motionStart ? "MOVE START" : motionEnded ? " MOVE END " : isStopped() ? "   STOP   " : "   MOVE   ", CENTER, 113);
      if(!accError) {
        LCDprintNumI(secondsStopped(), RIGHT, 113, 5);
      }
      if(justStarted) {
        sprintf(logText + strlen(logText), ",INIT");
      } else {
        sprintf(logText + strlen(logText), accError ? ",ERROR" : motionStart ? ",MOVE_START" : motionEnded ? ",MOVE_END" : isStopped() ? ",STOP" : ",MOVE");
      }

    #else

      sprintf(logText + strlen(logText), ",DISABLED");
      
    #endif
    /*********************/

    //LOG 
    sprintf(logText + strlen(logText), ",%d", offlineMode ? 0 : xbeePowerLevel);

    if(bluetoothConnected){
      sendRemoteMessage(String("T" + String(logText)));
    }
    
    #ifdef TERMINAL_LOG
      Terminal.print(logText);
    #endif

    // SAVE LOG TO SD CARD FILE (DATE FILENAME)
    bool timeToWriteData = justStarted || motionStart || motionEnded || !isStopped() || timedOut(lastSdWriteAt, SDWriteIntervalSeconds) || gpsPositionAcquired || gpsPositionLost;
    if(writeSd && timeToWriteData) {
      if(writeSDline(logFileName, logText)) {
        Terminal.println(F("SD_OK "));
      } else {
        Terminal.println(F("SD_ERROR "));
      }
    } 

// ----------------------------------------------------------------
// OFFLINE MODE
// ----------------------------------------------------------------

    if(offlineMode && txEnabled && timedOut(lastConnectionAttemptAt, connectionRetryIntervalSeconds)) {

      if(sendXbeeData("", true)) { // First connection test
        delay(1000);
        if(sendXbeeData("", true)) { // Second connection test
          offlineMode = false;
        }
        
      } else {
        lastConnectionAttemptAt = millis();
      }

    }

//    // Send offline cache
    if(!offlineMode && txEnabled && isStopped() && (SD.exists(OFFLINE_FILE1) || SD.exists(OFFLINE_FILE2))) {

      File offlineLog, secondaryOfflineLog;
      bool fileOneExists = false;
      bool fileTwoExists = false;
      if(SD.exists(OFFLINE_FILE1)) {
        SD.remove(OFFLINE_FILE2);
        offlineLog = SD.open(OFFLINE_FILE1);
        fileOneExists = true;
      } else {
        if(SD.exists(OFFLINE_FILE2)) {
          offlineLog = SD.open(OFFLINE_FILE2);
          fileTwoExists = true;
        }
      }
      
      if(offlineLog) {
  
        uint32_t totalSentSize = 0;
        uint8_t progress = 0;
  
        String offlineText = "SENDING\n\"";
        offlineText += offlineLog.name();
        offlineText += "\"\n";
        offlineText += offlineLog.size();
        offlineText += " bytes";
        
        LCD_ON;
        LCDprintScreen(offlineText.c_str() , CENTER, false, LCD_LINE_GAP);
        LCDprint("  0 %", CENTER, 80);
        lcd.drawBitmap(5, 95, 25, 29, striderSmall); // Print Strider Logo // drawBitmap (x, y, sx, sy, data[, scale]);
  
        Terminal.println(offlineText);
        
        while(offlineLog.available()) {
  
          String offlineLogLine = offlineLog.readStringUntil('\r'); // DANGER - THIS CAN CAUSE AN BUFFER OVERFLOW AND RESTART SOFTWARE IF LINE IS TOO BIG
          offlineLogLine.replace("\r", "");
          offlineLogLine.replace("\n", "");
          
          if(!offlineMode && offlineLogLine.length() > 4) {
            offlineMode = !sendXbeeDataTotal(offlineLogLine.c_str(), true, &totalSentSize, 0);
            Terminal.print("-> ");
            Terminal.println(offlineLogLine);
  
            progress = ((double) totalSentSize/offlineLog.size()) * 100;
  
            LCDprintNumI(progress, CENTER, 80, NULL);
          }
  
          if(offlineMode && offlineLogLine.length() > 4) {
  
            if(!secondaryOfflineLog) {
              
              if(fileOneExists) {
                secondaryOfflineLog = SD.open(OFFLINE_FILE2, FILE_WRITE);
              } else {
                secondaryOfflineLog = SD.open(OFFLINE_FILE1, FILE_WRITE);
              }
              if(secondaryOfflineLog) {
                Terminal.print(F("Communication error sending old logs... copying to secondary file: "));
                Terminal.println(secondaryOfflineLog.name());
              }
              
            }
  
            if(secondaryOfflineLog) {
              secondaryOfflineLog.println(offlineLogLine);
              Terminal.print(F("<> "));
              Terminal.println(offlineLogLine);
            }
            
          }
          
        } // while end
        
        offlineLog.close();
        if(secondaryOfflineLog) secondaryOfflineLog.close();
  
        if(fileOneExists) SD.remove(OFFLINE_FILE1);
        if(fileTwoExists) SD.remove(OFFLINE_FILE2);
  
        LCD_OFF;
        
      }
      
    }
    
    String offlineFileName = OFFLINE_FILE1;
    if(SD.exists(OFFLINE_FILE2) && !SD.exists(OFFLINE_FILE1)) {
      offlineFileName = OFFLINE_FILE2;
    }

    bool timeToSendData = justStarted || motionStart || motionEnded || (!isStopped() && timedOutMs(lastReportAt, reportInterval)) || timedOut(lastReportAt, reportStoppedInterval) || gpsPositionAcquired || gpsPositionLost;

// ----------------------------------------------------------------
// ONLINE MODE
// ----------------------------------------------------------------

    if(!offlineMode) {
      
      // if offlineMode is cleared send log
      // if error (2 time retry) set offlineMode = TRUE

      if(timeToSendData && txEnabled) {

        bool successTx = false;
        int tryCount = 0;
        while(!successTx && tryCount++ < 2) { // Try to send data 2 times
          successTx = sendXbeeData(logText, true);
        }
        Terminal.print("XBEE_");
        Terminal.println(successTx ? "OK " : "ERROR ");

        if(successTx) {
          lastReportAt = millis();
        } else {
          lastConnectionAttemptAt = millis();
          offlineMode = true;
        }
        
      }   
            
    } 

    //
    if(offlineMode && timeToSendData) {
      
      if(writeSDline(offlineFileName.c_str(), logText)) {
        Terminal.print(F("SD_"));
        Terminal.print(offlineFileName);
        Terminal.print(F(" "));
      } else {
        Terminal.print(F("SD_ERROR "));
      }

      lastReportAt = millis();
      
    }

    if(justStarted) {
      justStarted = false;
      writeSDline("STARTUP.TXT", currentDateTime);
    }

    #ifdef TERMINAL_LOG
      Terminal.println();
    #endif
    
    switchLed();
    
  }

}
