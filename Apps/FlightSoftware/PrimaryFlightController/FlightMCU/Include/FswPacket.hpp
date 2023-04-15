#ifndef _FSW_PACKET_H_
#define _FSW_PACKET_H_

// #define NETWORK_TEST_ENV_TEST
#define NETWORK_TEST_ENV_MOON

// Wifi Connection Parameters
#ifdef NETWORK_TEST_ENV_MOON
#define ROVER_ADDRESS    \
    {                    \
        192, 168, 150, 3 \
    }
#define SPACECRAFT_ADDRESS \
    {                      \
        192, 168, 10, 105  \
    }
#define SUBNET_MASK      \
    {                    \
        255, 255, 255, 0 \
    }
#define GATEWAY_ADDRESS    \
    {                      \
        192, 168, 150, 254 \
    }
#define SPACECRAFT_UDP_PORT 43531
#define ROVER_UDP_PORT 42000

#define LANDER_SSID "PM1LWAP"
#define LANDER_NETWORK_PASSWORD "PIMS_WAP"
#else // local testing (we should migrate away from needing this)
#define ROVER_ADDRESS  \
    {                  \
        192, 168, 1, 2 \
    }
#define SPACECRAFT_ADDRESS \
    {                      \
        192, 168, 1, 120   \
    }
#define SUBNET_MASK      \
    {                    \
        255, 255, 255, 0 \
    }
#define GATEWAY_ADDRESS  \
    {                    \
        192, 168, 1, 120 \
    }
#define SPACECRAFT_UDP_PORT 8080
#define ROVER_UDP_PORT 8080

#define LANDER_SSID "Houston"
#define LANDER_NETWORK_PASSWORD "redr0ver"

#endif

#define INITIAL_PRIMARY_NETWORK_INTERFACE WATCHDOG // WATCHDOG or WF121 | Must be of type PrimaryInterface (see GroundInterfaceComponentAi.xml or GroundInterfaceComponentAc.hpp)

// Packet sizes
#define IPV4_MTU 1006 // IDD Section 5.2.3 (M-PE1-CS-0100G) Table 5 IETC RFC 791 **FRAGMENTATION *NOT* SUPPORTED**
// Static buffer sizes (MAXIMUM ALLOCATION)
#define WF121_UDP_MAX_PAYLOAD (IPV4_MTU - 20 - 8) // IDD Section 5.2.3 (M-PE1-CS-0100G) Table 5 IETC RFC 768 (20byte IPv4 header, 8byte UDP Header)
#define WATCHDOG_MAX_PAYLOAD 650                  // Watchdog UDP Buffer Size
#define NUM_APPS_USE_FILE_DOWNLINK 2              // Sets the total number of of static downlink buffers (one per application)
                                                  // Since buffers are statically allocated can't use get_appDownlink_Ports()
                                                  // Camera: 0       UWB:1   <- Port Number indexes which application is sending data and which buffer to use

// FSW Packet Magic (32bit)
#define FSW_COMMAND_MAGIC 0x00bada55
#define FSW_RADIO_COMMAND_MAGIC 0xd009ad10 // Magic used for commands sent to the Radio. Due to the way the Radio intercepts them, they'll get through to us but we can safely ignore them.
#define FSW_LOG_MAGIC 0x0deadbad
#define FSW_TELEMETRY_MAGIC 0xc00010ff
#define FSW_FILE_MAGIC 0xdabad000
#define FSW_COMMAND_RESPONSE_MAGIC 0xc001d00d

namespace FswPacket
{

    typedef uint8_t Seq_t;
    typedef uint16_t Length_t;
    typedef uint8_t Checksum_t;
    typedef uint32_t Magic_t;
    typedef uint8_t Component_t;
    typedef Length_t FileLength_t;

    struct FswPacketHeader
    {
        Seq_t seq;
        Length_t length;
        Checksum_t checksum;
    } __attribute__((packed));

    struct FswCommand
    {
        Magic_t magic;
        uint8_t opcode;        // id when concatenated resulting in a U16
        Component_t component; // This field along with command make up the
        uint8_t byte0;
    } __attribute__((packed));

    struct FswCommandResponse
    { // This is downlinked via the file (app downlink port)
        Magic_t magic;
        uint8_t opcode;        // Same as command
        Component_t component; // Same as command
        uint8_t errorcode;
        uint16_t errorinfo;
    } __attribute__((packed));

    struct FswFileHeader
    {
        Magic_t magic;
        uint16_t fileGroupId;         // Which "File Group" this came from. Since we're downlinking image lines as a "File", we need a way for grouping them together so we know they're part of the same image. This is a hash of the image capture time. Two images over the course of the entire mission could have the same `fileGroupId`. This is only unique up to the point that no two files with overlapping or adjacent transmission periods should have the same `fileGroupId`.
        uint16_t fileGroupLineNumber; // This tells us distinctly which line in the "File Group" (image) this "File" is. Used to differentiate blocks so we know which file they came from. This is 0-indexed. (NOTE: This replaces the old hashedId.)
        uint8_t totalBlocks;
        uint8_t blockNumber; // This is 1-indexed, 0-index is optional and contains file-specific metadata
        FileLength_t length; // This is the size of the following data **not including this header**
    } __attribute__((packed));

    struct FswFileMetadata
    { // Block 0 of files
        uint16_t callbackId;
        uint32_t timestamp; // when the file / file group was created (image capture time)
    } __attribute__((packed));

    struct FswFile
    {
        struct FswFileHeader header;
        union
        {
            uint8_t byte0;
            struct FswFileMetadata metadata;
        } file;
    } __attribute__((packed));

    struct FswPacket
    {
        struct FswPacketHeader header;
        union
        {
            uint8_t startByte;
            Magic_t magic0; // Magic of first packet
            // FSWPacket Object headers
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
