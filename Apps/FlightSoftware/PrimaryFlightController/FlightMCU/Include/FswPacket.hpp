#ifndef _FSW_PACKET_H_
#define _FSW_PACKET_H_

// Wifi Connection Parameters
#define ROVER_IP_ADDRESS        {192, 168, 1, 2}
#define ROVER_MASK_ADDRESS      {255, 255, 255, 0}
#define ROVER_GATEWAY_ADDRESS   {192, 168, 1, 1}
#define GATEWAY_PORT            8080
#define ROVER_UDP_PORT          8080 

#define LANDER_SSID             "Houston"
#define LANDER_NETWORK_PASSWORD "redr0ver"

// Wired (RS422 via WatchDog) Connection Parameters
#define WIRED_UDP_PORT_ROVER    8080
#define WIRED_UDP_PORT_LANDER   8080

// Packet sizes
#define IPV4_MTU                1006    // IDD Section 5.2.3 (M-PE1-CS-0100G) Table 5 IETC RFC 791 **FRAGMENTATION *NOT* SUPPORTED**
#define UDP_MAX_PAYLOAD         978     // IDD Section 5.2.3 (M-PE1-CS-0100G) Table 5 IETC RFC 768
// 1006byte - 20byte IPv4 header - 8byte UDP header = 978byte payload

// FSW Packet Magic (32bit)
#define FSW_COMMAND_MAGIC           0x00bada55
#define FSW_LOG_MAGIC               0x0deadbad
#define FSW_TELEMETRY_MAGIC         0xc00010ff
#define FSW_FILE_MAGIC              0xdabad000
#define FSW_COMMAND_RESPONSE_MAGIC  0xc001d00d

namespace FswPacket {

typedef uint8_t Seq_t;
typedef uint16_t Length_t;
typedef uint16_t Checksum_t;
typedef uint32_t Magic_t;
typedef uint8_t Component_t;
typedef Length_t FileLength_t;

struct FswPacketHeader {
    Seq_t seq;
    Length_t length;
    Checksum_t checksum;
} __attribute__((packed));

struct FswCommand {
    Magic_t magic;
    Component_t component;      // This field along with command make up the
    uint8_t opcode;         // id when concatenated resulting in a U16
    uint8_t byte0;
} __attribute__((packed));

struct FswCommandResponse { // This is downlinked via the file (app downlink port)
    Magic_t magic;
    Component_t component;      // Same as command
    uint8_t opcode;         // Same as command
    uint8_t errorcode;
    uint16_t errorinfo;
} __attribute__((packed));

struct FswFile {
    Magic_t magic;
    uint8_t totalBlocks;
    uint8_t blockNumber;    // This is 1-indexed
    FileLength_t length;        // This is the size of the following data **not including this header**
    uint8_t byte0;
} __attribute__((packed));
    
struct FswPacket {
    struct FswPacketHeader header;
    union {
       uint8_t startByte;
       Magic_t magic0;     // Magic of first packet
       struct FswCommand command;
       struct FswCommandResponse cmdResp;
       struct FswFile file;
       // struct FswTelemetry telemetry;
       // struct FswLog log;
    } payload0;
    // Additional telemetry or logs (but not commands) can follow the first
    // object in the payload, payload0, up until the end of the packet determined
    // by length.
} __attribute__((packed));

} // namespace FswPacket

#endif // _FSW_PACKET_H_
