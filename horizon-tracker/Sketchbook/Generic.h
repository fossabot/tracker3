//Include system libs
#include <stdint.h>
#include <avr/pgmspace.h>

#ifndef __HorizonGeneric_h
#define __HorizonGeneric_h
//DEFINE DEBUG OPTIONS

#define SHOW_LOG true
#define SHOW_INFO false
#define SHOW_TODO false
#define SHOW_DEBUG false
#define SHOW_WARNING false
#define SHOW_ERROR false
#define SHOW_FATAL_ERROR false

#define LEVEL_LOG 0
#define LEVEL_TODO 1
#define LEVEL_DEBUG 2
#define LEVEL_WARNING 3
#define LEVEL_ERROR 4
#define LEVEL_FATAL_ERROR 5
#define LOG Terminal.print(F("\033[1;30mLOG\033[0m: "))
#define INFO Terminal.print(F("\033[1;30mINFO\033[0m: "))
#define TODO Terminal.print(F("\033[1;36mTODO\033[0m: "))
#define DEBUG Terminal.print(F("\033[1;34mDEBUG\033[0m: "))
#define WARNING Terminal.print(F("\033[1;33mWARNING\033[0m: "))
#define ERROR Terminal.print(F("\033[1;31mERROR\033[0m: "))
#define FATAL_ERROR Terminal.print(F("\033[1;41mFATAL ERROR\033[0m: "))


#define SW_TRACKER_VERSION 36
const PROGMEM uint32_t VERSION = SW_TRACKER_VERSION;
#define ORB true

//#define XBEE_DEST           "0013A2004103569E" // LOCAL TEST
//#define XBEE_DEST           "0013A20041084E13"
#define XBEE_DEST           "0013A200415B7A98" // TSUGE LOTE 15
//#define XBEE_DEST           "0013A2004107265B" // SANTA ELIZA
//#define XBEE_DEST           "0013A20041072659" // VELOSO
//#define XBEE_DEST           "0013A20041084E1F"
#define XBEE_BROADCAST_ADD  "000000000000FFFF"
#define XBEE_MODE           '1' // '0'= Transparent mode / '1' = API Mode

#define PRIMARY_FILE "OFFLINE1.TXT"
#define SECONDARY_FILE "OFFLINE2.TXT"
#define STATUS_FILE "STATUS.TXT"
#define FILE_POSITION "POSITION.TXT"
#define LOG_DIR "/LOG"
#define LOG_EXT ".LOG"


#define LCD 1
#define GPS 2 // (1 = GPS uBlox NEO-M8 / 2 = GPS NEO-6M / RoyalTek REB-5216)
#define BLUETOOTH 1 //(1 = HC05 / 2 = HM10 / 3 = HC06)
#define XBEE 2 // 1 = Transparent mode / 2 = API Mode
#define ACCELEROMETER 1
#define SDCARD 1
//#define WRITE_EEPROM
//#define CLEAR_EEPROM
//#define ORB
//#define DISCOVER_DEVICES

#define STRIDER_BOARD

//DEFINE CONSTANTS
#ifdef STRIDER_BOARD
#define SerialXBee                    Serial2
#define SerialBT                      Serial3
#define ACCELEROMETER_INTERRUPT_PIN   2   // D2 - PE4
#define DE_RE_485                     7   // D7 - PH4 / 485 DE/!RE pin
#define LCD_SWITCH_PIN                25//26  // D26 - PA4 / ITDB02 Secondary interface pin BL  (PLACA V2 25//)
#define XBEE_RESET                    8//25  // D25 - PA3                                   (PLACA V2 8//)
//    #define XBEE_DOUT                     15 // RXD3/PCINT9 (USART3 Receive Pin or Pin Change Interrupt 9)
#define POWER_XBEE                    9  // ?
#define POWER_GPS                     40  // ?
#define POWER_485                     6  // D6 - PH3
#define POWER_BLUETOOTH               38  // D38 - PD7
#define BT_MODE                       12 //D12 pin25//(PLACA V2)
#define BT_RST                        41 //D12 pin25//(PLACA V2)
#define GPS_BAUD_RATE 9600
#define XBEE_RTS                      A1
#else
#define SerialXBee                    Serial3
#define SerialBT                      Serial
#define SerialInput                   SerialXBee
#define SerialCommunication           SerialXBee
#define ACCELEROMETER_INTERRUPT_PIN   A8 // EB8
#define DE_RE_485                     A12 // 485 DE/!RE pin
#define LCD_SWITCH_PIN                38 // ITDB02 Parallel LCD Module Interface - PIN 7
#define XBEE_RESET                    49
//    #define XBEE_DOUT                     15 // RXD3/PCINT9 (USART3 Receive Pin or Pin Change Interrupt 9)
#endif


#define TERMINAL_BAUD_RATE            115200
#define RS485_BAUD_RATE               115200
#define ONE_WIRE_BUS                  5   // Temperature sensor
#define LED                           13  // LED_TEST
#define SD_CS                         4   // D4 - PG5
#define PLUVIOMETER_INTERRUPT         A13 // EB13 -> PK5
#define Terminal                      Serial
#define SerialGPS                     Serial1 // Same serial, use one or another
#define Serial485                     Serial1 // Same serial, use one or another

#define AT_BUFFER_ZISE  64
#define TIME_OUT_AT     3000
#define TIME_OUT_SERIAL 100
#define MESSAGE_BUFFER_SIZE 256
#define PACKET_SENT_TIME_OUT 5000
//#define CONNECTION_RETRY_INTERVAL 60 //sec

// macros from DateTime.h
/* Useful Constants */
#define SECS_PER_MIN  (60UL)
#define SECS_PER_HOUR (3600UL)
#define SECS_PER_DAY  (SECS_PER_HOUR * 24L)

/* Useful Macros for getting elapsed time */
#define numberOfSeconds(_time_)       (_time_ % SECS_PER_MIN)
#define numberOfMinutes(_time_)       ((_time_ / SECS_PER_MIN) % SECS_PER_MIN)
#define numberOfHours(_time_)         (( _time_% SECS_PER_DAY) / SECS_PER_HOUR)
#define elapsedDays(_time_)           ( _time_ / SECS_PER_DAY)

#define ON          HIGH
#define OFF         LOW
#define LED_ON      digitalWrite(LED, ON);
#define LED_OFF     digitalWrite(LED, OFF);

#define CMD_ABORT                     "ABORT"
#define CMD_OK                        "OK"
#define CMD_CRC_ERROR                 "CRC_ERROR"
#define CMD_UPDATING                  "UPDATING"
#define CMD_RESET                     "RESETTING"

#define MINIMUM_REPORT_INTERVAL       1000 // 1 second
#define STOP_TIMEOUT 5 //300 // How long (in seconds) between last motion to consider device as STOPPED ->
#define DEFAULT_REPORT_INTERVAL 2500 //in ms
#define STOP_TRACK_INTERVAL 300 // When device is stopped it should send data in this interval (seconds)
#define MINIMUM_MOVEMENT_TIME 5 // Minimum amount of time the device must be in movement to turn on the lcd



#define CR 0x0D
#define LF 0x0A



//DEFINE GLOBAL VARIABLES

// System flags with initial values
extern uint32_t lcdOnAt;
extern uint32_t lcdOffAt;
extern uint32_t reportInterval;
extern uint32_t reportStoppedInterval;
extern uint32_t offlineDataDisableAt;
extern uint32_t centralAliveAt;
extern int currentLine;
extern bool enableTx;
extern bool enableOfflineData;
extern bool enableOfflineDataSend;
extern unsigned long logFileNumber;
extern bool bluetoothConnected;
extern bool justStarted;
extern bool horizonConnected;
extern bool motionEnded;
extern bool motionStart;
extern int batteryLevel;
extern bool gpsAntennaRemoved;
extern bool logSaved;

const int EEPROM_MIN_ADDR = 0;
const int EEPROM_MAX_ADDR = 4095;


// Include libraries

#include "Functions.h"
//#include "Time.h"
//#include <DS1307RTC.h>
#include "EEPROM.h"
#include <avr/wdt.h>
#include <PinChangeInterrupt.h>
#include <PinChangeInterruptBoards.h>
#include <PinChangeInterruptPins.h>
#include <PinChangeInterruptSettings.h>
#include <MemoryFree.h>
#include "EepromUtil.h"
#include "LCD.h"
#include "XBee.h"
#include "Bluetooth.h"

#ifdef ACCELEROMETER
#include "Accelerometer.h"
#endif

#ifdef SDCARD
#include <SD.h>
#include <xmodem.h>
#include <CRC32.h>
#include "SDutils.h"
#include <TrueRandom.h>
#endif

// LIBRARY TO TEST -> https://github.com/stevemarple/MicroNMEA
//#include <TinyGPS++.h> // http://arduiniana.org/libraries/tinygpsplus/
#include "GPS.h"

#ifdef _485
#include "Epsolar.h"
#include <OneWire.h>
//#include <DallasTemperature.h>
#endif

#include "Commands.h"
extern nmGps::HorizonGPS                        horizonGPS;
extern nmBluetooth::HorizonBluetooth            horizonBluetooth;
extern nmXbee::HorizonXbee                      horizonXbee;
extern nmAccelerometer::HorizonAccelerometer    horizonAccelerometer;

struct MonitorData {
    char            stationType;
    int             firmwareVersion;
    char            bluetoothSerial[12];
    char            accelerometerStatus;
    int             signalPower;
    byte            systemStatus;
    byte            batteryLevel;
    byte            gpsStatus;
    unsigned short  year;
    unsigned char   month;
    unsigned char   day;
    unsigned char   hour;
    unsigned char   min;
    unsigned char   sec;
    unsigned char   gpsFixType;
    long            lat;
    long            lon;
    long            alt;
    long            headMot;
    unsigned long   hAcc;
    unsigned long   vAcc;
    long            gSpeed;
    unsigned long   sAcc;
    long            headVeh;
    short           magDec;
    unsigned short  magAcc;
    unsigned char   fusionMode;
public:
    int     toCharArray(char *data);
    int     toByteArray(char *data);
    bool    update();
    bool    updateGpsData();
    bool    updateCalibStatus();
    void    print();
    void    clear();
};

extern MonitorData monitorData;

//SYTEM STATUS MASK
#define SD_OK       0x01
#define SD_ERROR    0xFE
#define BT_CON      0x02
#define BT_NCON     0xFD
#define XB_CON      0x04
#define XB_NCON     0xFB
#define HR_CON      0x08
#define HR_NCON     0xF7
#define OFF_DATA    0x10
#define NO_OFF_DATA 0xEF
#define LOG_SAVED   0x20
#define LOG_FAIL    0xDF

//GPS STATUS MASK
#define UBX         0x01
#define NMEA        0xFE
#define ANT_OK      0x02
#define ANT_ERR     0xFD
#define TIME_VALID  0x0C
#define GNSS_FIX    0x10
#define HEAD_VEH    0x20

#endif // def(__HorizonGeneric_h)
