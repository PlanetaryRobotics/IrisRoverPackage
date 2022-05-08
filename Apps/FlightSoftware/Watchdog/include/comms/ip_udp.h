#ifndef __WATCHDOG_IP_UDP_H__
#define __WATCHDOG_IP_UDP_H__

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @defgroup watchdogIpUdp IP/UDP
 * @addtogroup watchdogIpUdp
 * @{
 */

#define iup_get_ver(pckt) (pckt->ver_hdrlen >> 4)
#define iup_get_hdrlen(pckt) (pckt->ver_hdrlen & 0xF)
#define iup_as_bytes(pckt) ((unsigned char *)pckt)
#define iup_get_payload(pckt) ((unsigned char *)(pckt + 1))

/* byte order swapping - msp430 is little endian; networks use big endian */
#define ntohs(x) ((x) >> 8 | ((x & 0xFF) << 8))
#define htons(x) ntohs(x)

struct __attribute__((__packed__)) ip_hdr {
    /* -=-=-=-=-=-=-=-=[ IP header ]=-=-=-=-=-=-=-=- */
    /* version and header length (use iup_get_ver() and iup_get_hdrlen() to
       get the appropriate values) */
    /* header length is usually 20 bytes = 5 */
    uint8_t ver_hdrlen;
    /* priority and type of service (ignored) */
    uint8_t tos;
    /* length of the entire packet */
    uint16_t pckt_len;
    /* packet id */
    uint16_t id;
    /* flags (ignored) */
    uint16_t flgs;
    /* TTL */
    uint8_t ttl;
    /* protocol (expected to be 0x11 for UDP) */
    uint8_t proto;
    /* checksum  (compute using) */
    /* TODO: compute checksum */
    uint16_t iphdr_checksum;
    /* source IP address */
    uint32_t source;
    /* destination IP address */
    uint32_t dest;
};

struct __attribute__((__packed__)) ip_pseudohdr {
    /* -=-=-=-=-=-=-=-=[ IP pseudoheader ]=-=-=-=-=-=-=-=- */
    /* source IP address */
    uint32_t source;
    /* destination IP address */
    uint32_t dest;
    /* pad */
    uint8_t zero;
    /* protocol (expected to be 0x11 for UDP) */
    uint8_t proto;
    /* length */
    uint16_t udp_len;
};

struct __attribute__((__packed__)) udp_hdr {
    /* -=-=-=-=-=-=-=-=[ UDP header ]=-=-=-=-=-=-=-=- */
    /* source port */
    uint16_t source_port;
    /* destination port */
    uint16_t dest_port;
    /* length */
    uint16_t len;
    /* checksum */
    uint16_t checksum;
};

struct __attribute__((__packed__)) checksum_udp_pckt {
    /* padding */
    uint32_t pad;
    /* -=-=-=-=-=-=-=-=[ IP header ]=-=-=-=-=-=-=-=- */
    struct ip_pseudohdr ip_pseudohdr;
    /* -=-=-=-=-=-=-=-=[ UDP header ]=-=-=-=-=-=-=-=- */
    struct udp_hdr udp_hdr;
    /* -=-=-=-=-=-=-=-=[ data here ]=-=-=-=-=-=-=-=- */
};

struct __attribute__((__packed__)) ip_udp_pckt {
    /* -=-=-=-=-=-=-=-=[ pseudo IP header ]=-=-=-=-=-=-=-=- */
    struct ip_hdr ip_hdr;
    /* -=-=-=-=-=-=-=-=[ UDP header ]=-=-=-=-=-=-=-=- */
    struct udp_hdr udp_hdr;
    /* -=-=-=-=-=-=-=-=[ data here ]=-=-=-=-=-=-=-=- */
};

/**
 * @brief Constants related to IP/UDP packets and/or their headers.
 */
typedef enum IpUdp__Constants
{
    /**
     * @brief The total combined length of the IP and UDP headers.
     */
    IP_UDP_HEADER_LEN = sizeof(struct ip_hdr) + sizeof(struct udp_hdr)
} IpUdp_Constants;

/**
 * @brief Possible return statuses of IpUdp functions.
 */
typedef enum IpUdp__Status {
    IP_UDP__STATUS__SUCCESS = 0, //!< The function completed successfully.

    IP_UDP__STATUS__ERROR_NULL = -1, //!< A required argument  or a member of an argument was NULL.
    IP_UDP__STATUS__ERROR_BUFFER_TOO_SMALL = -2, //!< A given buffer was too small for some purpose.
    IP_UDP__STATUS__ERROR_SERIALIZATION_FAILURE = -3//!< A Serialization function call returned an error.
} IpUdp__Status;

/**
 * @brief Given a serialized IP/IDP packet, returns a pointer to the beginning of the UDP payload of that packet along
 *        with the size of that payload.
 *
 * @param fullIpUdpPacketData The full IP/UDP packet data.
 * @param fullDataLen The length of `fullIpUdpPacketData`.
 * @param udpDataPointer An output parameter set to the beginning of the payload data.
 * @param udpDataSize An output parameter set to the size of the payload data.
 *
 * @return One of the following:
 *   - IP_UDP__STATUS__SUCCESS: The function was successful.
 *   - IP_UDP__STATUS__ERROR_NULL: `fullIpUdpPacketData`, `udpDataPointer`, or `udpDataSize` was NULL.
 *   - IP_UDP__STATUS__ERROR_BUFFER_TOO_SMALL: `fullIpUdpPacketData` was smaller than `IP_UDP_HEADER_LEN`.
 */
IpUdp__Status IpUdp__identifyDataInUdpPacket(uint8_t* fullIpUdpPacketData,
                                             size_t fullDataLen,
                                             uint8_t** udpDataPointer,
                                             size_t* udpDataSize);

/**
 * @brief Generates an IP and UDP header for the given payload data and serializes theses headers into the given
 *        output buffer.
 *
 * @param udpData The payload data to be used as the payload of the UDP packet.
 * @param udpDataSize The size of `udpData`.
 * @param serializationBuffer The output buffer into which the IP and UDP header will be serialized.
 * @param bufferLen The size of `serializationBuffer`.
 * @param packetId The value to use for the ID field of the IP header.
 *
 * @return One of the following:
 *   - IP_UDP__STATUS__SUCCESS: The function was successful.
 *   - IP_UDP__STATUS__ERROR_NULL: `udpData` or `serializationBuffer` was NULL.
 *   - IP_UDP__STATUS__ERROR_BUFFER_TOO_SMALL: `bufferLen` was smaller than `IP_UDP_HEADER_LEN`.
 */
IpUdp__Status IpUdp__generateAndSerializeIpUdpHeadersForData(const uint8_t* udpData,
                                                             size_t udpDataSize,
                                                             uint8_t* serializationBuffer,
                                                             size_t bufferLen,
                                                             uint16_t packetId);

/**
 * @}
 */

#ifdef __cplusplus
} /* close extern "C" */
#endif


#endif /* __WATCHDOG_IP_UDP_H__ */
