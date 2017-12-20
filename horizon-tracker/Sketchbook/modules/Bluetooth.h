#include "Arduino.h"
#include "../Generic.h"

#ifndef __HorizonBluetooth_h
#define __HorizonBluetooth_h

#define BT_BAUD_RATE      9600

#define BT_AT             "AT"
#define BT_MAC            "AT+ADDR?"
#define BT_CONNECTED_ADDR "AT+RADD?"

#define BT_MODE_TX        "AT+MODE0"
#define BT_MODE_REMOTE    "AT+MODE1"
#define BT_MODE_TX_REMOTE "AT+MODE2"

#if BLUETOOTH == 1
  #define BT_SET_NAME       "AT+NAME="//(HC05)
#else
  #define BT_SET_NAME       "AT+NAMEHR"//(HM10)
#endif

// If this value is set to 1, when link ESTABLISHED or LOSTED module will send OK+CONN or OK+LOST string through UART.
#define BT_NOTIFY_OFF     "AT+NOTI0"
#define BT_NOTIFY_ON      "AT+NOTI1"

// Query/Set Module Power
#define BT_PWR_0_DBM      "AT+POWE2" // Power 0 dBm
#define BT_PWR_6_DBM      "AT+POWE3" // Power 6 dBm

// Query/Set Module Bond Mode
#define BT_PIN_OFF        "AT+TYPE0" // Disable PIN
#define BT_PIN_ON         "AT+TYPE1" // Enable PIN

// REMOTE CMDS

// AT+RSSI?
// Query RSSI Value
// This command only used by Remote device query when connected.

//HC 05 Commands

#define BT_ROLE "AT+ROLE?"
#define BT_CMODE "AT+CMODE?"
#define BT_INIT_SPP "AT+INIT"
#define BT_UART_INQ "AT+UART?"
#define BT_STATE_INQ "AT+STATE?"

#define BT_CLASS "AT+CLASS?"
#define BT_DISC "AT+DISC"

#define BT_END char(0xC0)
#define BT_EOT "\r\n"

#define LOCAL_IDENTIFIER int(0x0A)
#define REMOTE_IDENTIFIER int(0xC0)
#define MESSAGE_SEPARATOR int(0x03)

#define MSG_NONE 0
#define MSG_LOCAL 1
#define MSG_REMOTE 2

#define CMD_LOCAL           'L'
#define CMD_PARAMETERS      'P'
#define CMD_XBEE_ACCES      'X'
#define CMD_XBEE_PAYLOAD    'M'
#define CMD_GPS_UPDATE      'G'

#define CMD_ACK             'A'
#define CMD_NACK            'N'
#define CMD_ERROR           'E'
#define CMD_MONITOR         'T'

namespace nmBluetooth {
    struct BluetoothPacket {
        char    packetCommand;
        int     packetSize;
        char    packetPayload[MESSAGE_BUFFER_SIZE];
    public:
        void    setCommand(char command);
        void    setSize(int size);
        bool    setPayload(char *payload, int size);
        char    getCommand() { return packetCommand; }
        int     getSize() { return packetSize; }
        char    *getPayload() { return packetPayload; }
        void    clear();
        
        bool    parse(int type, char *data, int size);
        void    print();
    };
    
    class HorizonBluetooth {
        char    bluetoothSerial[13];
        String  parseSerial(char *serial);
    public:
        BluetoothPacket bluetoothPacket;

        HorizonBluetooth();
        bool    setupBluetooth(void);
        bool    sendAtCommand(String cmd, char * response);
        int     getBluetoothMessage();
        bool    sendMessage();
        char    *getSerial() { return bluetoothSerial; }
        bool    tryGetSerial();
        void    newPacket(char command, char *payload, int size);
    };
}


#endif /* __HorizonBluetooth_h */
