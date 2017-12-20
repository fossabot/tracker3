#include "Arduino.h"
#include "../Generic.h"

#ifndef __HorizonXbee_h
#define __HorizonXbee_h

//#define XBEE_TX_BUFFER              500
//#define XBEE_RX_BUFFER              500
//#define RESPONSE_BUFFER_SIZE        500
#define XBEE_BAUD_RATE              115200

// API FRAME TYPES
#define START_DELIMITER             0x7E

#define PACKET_AT_CMD                      0x08
#define PACKET_AT_CMD_QUEUE_REG_VAL        0x09
#define PACKET_TX_REQUEST                  0x10
#define PACKET_EXPLICIT_ADDRESS_CMD_FRAME  0x11
#define PACKET_REMOTE_AT_CMD               0x17
#define PACKET_AT_CMD_RESPONSE             0x88
#define PACKET_MODEM_STATUS                0x8A
#define PACKET_TX_STATUS                   0x8B
#define PACKET_ROUTE_INFO_PACKET           0x8D // DigiMesh only
#define PACKET_AGGREGATE_ADDRESSING_UPDATE 0x8E // DigiMesh only
#define PACKET_RX_PACKET                   0x90
#define PACKET_EXPLICIT_RX_INDICATOR       0x91
#define PACKET_IODATA_SAMPLE_RX_INDICATOR  0x92 // DigiMesh only
#define PACKET_NODE_ID_INDICATOR           0x95
#define PACKET_REMOTE_AT_CMD_RESPONSE      0x97

#define TX_OPTIONS_DIGIMESH         0xC0
#define TRANSPARENT_MODE            '0'
#define API_MODE                    '1'

//PACKET SIZE AND OFFSET
//ALL PACKETS
#define PKT_DELIMITER_OFFSET 0
#define PKT_SIZE_OFFSET 1
#define PKT_TYPE_OFFSET 3
#define PKT_HEADER_SIZE 3
#define PKT_FRAMEID_OFFSET 4
#define PKT_DESTINATION_ADD_OFFSET 5
//RECEIVE PACKETS
#define TX_STS_PCKT_STATUS_OFFSET 8
#define RX_PKT_ADD_OFFSET 4
#define RX_PKT_DATA_OFFSET 15
#define RX_PKT_HEADER_SIZE 12
#define EXP_RX_PKT_DATA_OFFSET 21
#define EXP_RX_PKT_HEADER_SIZE 18
#define EXP_RX_PKT_SRC_EP_OSSSET 14
#define EXP_RX_PKT_DST_EP_OSSSET 15
#define EXP_RX_PKT_CLUSTERID_OFFSET 16
#define EXP_RX_PKT_PROFILEID_OFFSET 18
#define AT_CMD_RESP_PKT_CMD_OFFSET 5
#define AT_CMD_RESP_PKT_STATUS_OFFSET 7
#define AT_CMD_RESP_PKT_DATA_OFFSET 8
#define AT_CMD_RESP_PKT_HEADER_SIZE 5
//TRANSMIT PACKETS
#define AT_CMD_HEADER_SIZE 4
#define AT_CMD_COMMAND_OFFSET 5
#define AT_CMD_DATA_OFFSET 7
#define TX_HEADER_SIZE 14
#define TX_DATA_OFFSET 17
#define EXP_TX_SRC_EP_OFFSET 15
#define EXP_TX_DST_EP_OFFSET 16
#define EXP_TX_CLUSTERID_OFFSET 17
#define EXP_TX_PROFILEID_OFFSET 19
#define EXP_TX_HEADER_SIZE 20
#define EXP_TX_DATA_OFFSET 23

//STATUS
#define TRANSMIT_SUCCESS 0

// AT COMMANDS
#define ENTER_AT_CMD_MODE "+++"
#define EXIT_AT_CMD_MODE "ATCN"
#define OK "OK"
#define AT_SERIAL_HIGH "ATSH"
#define AT_SERIAL_LOW "ATSL"
#define AT_DST_SERIAL_HIGH "ATDH"
#define AT_DST_SERIAL_LOW "ATDL"
#define AT_FIND_NEIGHBORS "ATFN"
#define AT_NETWORK_DISCOVER "ATND"
#define AT_PREAMBLE_ID "ATHP"
#define AT_MODE "ATAP"
#define AT_TX_OPTIONS "ATTO"
#define AT_TX_OPTIONS_P2MP "ATTO40"
#define AT_TX_OPTIONS_REPEATER "ATTO80"
#define AT_TX_OPTIONS_DIGIMESH "ATTOC0"
#define AT_EXPLICIT_MODE "ATAO1"

//AT API COMMANDS
#define LAST_PACKET_RSSI "DB"

//SOURCE ENDPOINTS
#define SOURCE_ENDPOINT_DEFAULT    0xE8
#define SOURCE_ENDPOINT_CMD        0x0C
#define LOWER_SOURCE_ENDPOINT_ORB        0xA0
#define UPPER_SOURCE_ENDPOINT_ORB        0xC0
//CMDs (destination endpoint)
#define CMD_RESET               0x00
#define CMD_UPDATE              0x01
#define CMD_TX_ENABLE           0x02
#define CMD_TX_DISABLE          0x03
#define CMD_READ_PARAM          0x04
#define CMD_SET_DATA_RATE       0x05
#define CMD_SET_DEFAULT_CONFIG  0x06
#define CMD_LIST_SD             0x10
#define CMD_ERASE_SD            0x11
#define CMD_READ_FILE           0x12
#define CMD_REMOVE_FILE         0x13
#define CMD_OFFLINE_DATA        0x20
#define CMD_ALIVE               0x0A

namespace nmXbee{
    struct XbeePacket {
        uint16_t    packetLength;
        uint8_t     packetFrameType;
        char        packetPayload[MESSAGE_BUFFER_SIZE];
        char        packetCommand[2];
        char        packetRemoteAddress[8];
        uint8_t     packetFrameId;
        uint8_t     packetSourceEndpoint;
        uint8_t     packetDestinationEndpoint;
        uint16_t    packetClusterId;
        uint16_t    packetProfileId;
        uint8_t     packetStatus;
        uint8_t     packetChecksum;
    public:
        int         getLength() { return packetLength; }
        uint8_t     getFrameType()  { return packetFrameType; }
        char        *getPacketPayload() { return packetPayload; }
        char        *getPacketCommand() { return packetCommand; }
        uint8_t     getPacketStatus()   { return packetStatus; }
        char        *getPacketRemoteAddress()    { return packetRemoteAddress; }
        uint8_t     getPacketFrameId() { return packetFrameId; }
        uint8_t     getPacketSourceEndpoint() { return packetSourceEndpoint; }
        uint8_t     getPacketDestinationEndpoint() { return packetDestinationEndpoint; }
        uint16_t    getPacketClusterId() { return packetClusterId; }
        uint16_t    getPacketProfileId() { return packetProfileId; }
        
        bool    parse(char *data, int size);
        int     toArray(char *rawPacket);
        void    print();
        void    clear();
    };
    
    class HorizonXbee {
        bool    programable;
        char    operatingMode;
        bool    online;
        char    deviceAddress[16];
        char    deviceDestinationAddress[8];
        uint8_t powerLevel;
        bool    packetStored;
        
    public:
        XbeePacket xbeePacket;
        
        HorizonXbee();
        bool setupXBee(char *destinationAddress, char mode);
        bool enterAtMode(void);
        bool sendAtCommand(String cmd, char * response);
        bool setDestinationAddress(char *address);
        bool setDefaultDestinationAddress(char *address);
        bool setMode(char mode);
        bool tryGetSerial();
        bool getXbeePacket();
        bool sendPacket();

        bool isOnline() { return online; }
        char *getDeviceAddress() { return deviceAddress; }
        char *getDestinationAddress() { return deviceDestinationAddress; }
        uint8_t getPowerLevel() { return powerLevel; }
        void newPacket(uint8_t frameType, char *command, char *remoteAddress, uint8_t frameId, uint8_t sourceEndpoint, uint8_t destinationEndpoint, uint16_t clusterId, uint16_t profileId, char* data, int dataSize);
        bool analiseStoredPack() { return packetStored; }
        void setPacketStored(bool isStored);
        bool readPowerLevel();
    };
    
    
}

#endif /* __HorizonXbee_h */
