#include <stdint.h>
#include <string.h>

#include "comms/ip_udp.h"
#include "drivers/uart.h"
#include "cfg.h"

/* note - msp430 is little endian; networks are big endian */

/**
 * Return the IP checksum of the blob given, **in host byte order**
 */
static uint16_t ip_checksum(const uint8_t *packet, uint16_t packet_len) {
    uint16_t packet_len_orig;
    uint16_t i;
    uint16_t tmp;
    uint32_t accum = 0xffff;
    
    /* store the original packet length before we modify it */
    packet_len_orig = packet_len;

    // (packet_len & 1) is the same as (packet_len % 2)
    if (packet_len & 1) {
        /* ignore the last byte for now since we have to pad it later */
        packet_len -= 1;
    }

    /* loop through the complete multiples of 2 */
    for (i = 0; i < packet_len; i += 2) {
        /* set the tmp to be the 16 bit big endian value */
        /* note that this implicitly does the byte order swapping necessary */
        tmp = (packet[i] << 8) | packet[i + 1];

        /* add the value to the accumulator */
        accum += tmp;

        /* check if there has been an overflow */
        if (accum >= 0xffff) {
            /* wrap around if carry */
            accum -= 0xffff;
        }
    }

    /* check if we had to cut off a bit */
    if (packet_len != packet_len_orig) {
        /* the top half is the value from the packet, and the bottom half is
         * just 0
         */
        /* note that this implicitly does the byte order swapping necessary */
        tmp = (packet[packet_len] << 8);

        /* add the value to the accumulator */
        accum += tmp;

        /* check if there has been an overflow */
        if (accum >= 0xffff) {
            /* wrap around if carry */
            accum -= 0xffff;
        }
    }

    /* need to invert the accumulator to get the hash */
    return ~accum;
}

/**
 * Verify the values (including checksum) in an ip header.
 * 
 * Performs some basic validation of the input (e.g. packet length is long
 * enough, etc.) and then checks that the checksum = 0
 * 
 * Returns 0 if checksum is ok, and non-zero value if header or checksum has
 * some sort of error
 * (NOTE: there is no guarantee for the returned value to be any specific number
 * if there is an error in the checksum)
 */
static uint16_t ip_verify_packet(uint8_t *packet, uint16_t packet_len) {
    struct ip_hdr *header;
    uint16_t word_stor;

    /* check the size is at least minimally correct */
    if (packet_len < sizeof(struct ip_hdr)) {
        /* bad size */
        return 1;
    }

    /* check header version/length is right */
    if (*packet != 0x45) {
        /* either wrong version or wrong packet size (additional options are not
         * supported in this implementation) */
        return 2;
    }

    /* safe to cast now */
    header = (struct ip_hdr *)packet;

    /* ensure length is correct */
    word_stor = header->pckt_len;
    /* swap endianness of all values > 8 bits */
    word_stor = htons(word_stor);
    if (word_stor != packet_len) {
        /* packet reported length does not equal received length */
        return 3;
    }

    /* double-check the ip addresses are correct */
    if (header->source != LANDER_ADDRESS) {
        return 4;
    }
    if (header->dest != SPACECRAFT_ADDRESS) {
        return 5;
    }

    /* verify the checksum */
    word_stor = ip_checksum(packet, sizeof(struct ip_hdr));

    /* ip checksum should be 0 or 0xffff (negative 0) for a valid packet */
    return (word_stor == 0xffff || word_stor == 0x0) ? 0 : word_stor;
}

/**
 * calculate UDP checksum
 *
 * @param udp_header: udp header *only* (no ip header part)
 * @param data_buf: udp data buffer
 * @param udp_packet_len: length of udp header + data part
 * @param ip_src: source ip address
 * @param ip_dest: destination ip address
 */
static uint16_t udp_checksum(const uint8_t *udp_header,
                             const uint8_t *data_buf,
                             uint16_t udp_packet_len,
                             uint32_t ip_src,
                             uint32_t ip_dest) {
    struct ip_pseudohdr ph;
    uint32_t running_chksum;
    uint16_t chk_ret;

    ph.source = ip_src;
    ph.dest = ip_dest;
    ph.zero = 0;
    ph.proto = 0x11;
    ph.udp_len = udp_packet_len;

    udp_packet_len = htons(udp_packet_len);
    udp_packet_len -= sizeof(struct udp_hdr);

    // hash the pseudoheader first
    chk_ret = ip_checksum((uint8_t *)(&ph), sizeof(struct ip_pseudohdr));
    running_chksum = ~chk_ret;

    // then hash the udp header
    chk_ret = ip_checksum(udp_header, sizeof(struct udp_hdr));
    running_chksum += ~chk_ret;

    /* check if there has been an overflow */
    if (running_chksum >= 0xffff) {
        /* wrap around if carry */
        running_chksum -= 0xffff;
    }

    // finally, hash the data
    chk_ret = ip_checksum(data_buf, udp_packet_len);
    running_chksum += ~chk_ret;

    /* check if there has been an overflow */
    if (running_chksum >= 0xffff) {
        /* wrap around if carry */
        running_chksum -= 0xffff;
    }

    // return the final checksum
    return ~running_chksum;
}

IpUdp__Status IpUdp__identifyDataInUdpPacket(uint8_t* fullIpUdpPacketData,
                                             size_t fullDataLen,
                                             uint8_t** udpDataPointer,
                                             size_t* udpDataSize)
{
    if (NULL == fullIpUdpPacketData || NULL == udpDataPointer || NULL == udpDataSize) {
        return IP_UDP__STATUS__ERROR_NULL;
    }

    if (fullDataLen < IP_UDP_HEADER_LEN) {
        /* Input data is too small to contain a UDP packet inside of an IP packet*/
        return IP_UDP__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    //!< @todo Do we want to add validation of contents of IP and/or UDP headers?

    *udpDataPointer = fullIpUdpPacketData + IP_UDP_HEADER_LEN;
    *udpDataSize = fullDataLen - IP_UDP_HEADER_LEN;

    return IP_UDP__STATUS__SUCCESS;
}

IpUdp__Status IpUdp__generateAndSerializeIpUdpHeadersForData(const uint8_t* udpData,
                                                             size_t udpDataSize,
                                                             uint8_t* serializationBuffer,
                                                             size_t bufferLen,
                                                             uint16_t packetId)
{ 
    if (NULL == udpData || NULL == serializationBuffer) {
        return IP_UDP__STATUS__ERROR_NULL;
    }

    if (bufferLen < IP_UDP_HEADER_LEN) {
        return IP_UDP__STATUS__ERROR_BUFFER_TOO_SMALL;
    }

    struct ip_hdr *ip_hdr = (struct ip_hdr *) serializationBuffer;
    struct udp_hdr *udp_hdr = (struct udp_hdr *)(ip_hdr + 1);




     // NOTE: The code below is kind of gross, but it (presumably) works so I'm reusing it for now.
     //!< @todo Rewrite this code so it is neater and maybe less hardcoded.
     //
     // Old code below
     //==============================================================


    /* make the ip header first */
    ip_hdr->ver_hdrlen = 0x45;
    ip_hdr->tos = 0;
    ip_hdr->pckt_len = htons(udpDataSize + sizeof(struct ip_hdr) + sizeof(struct udp_hdr));
    ip_hdr->id = htons(packetId);
    ip_hdr->flgs = 0;
    // don't really care
    ip_hdr->ttl = 0xff;
    // udp = 0x11
    ip_hdr->proto = 0x11;
    // checksum is 0 for now
    ip_hdr->iphdr_checksum = 0;
    // addresses
    ip_hdr->source = 0x0267A8C0;
    ip_hdr->dest = 0x0167A8C0;
    // compute checksum
    uint16_t ipHeaderChecksum = ip_checksum(serializationBuffer, sizeof(struct ip_hdr));
    ip_hdr->iphdr_checksum = htons(ipHeaderChecksum);

    /* next, make the udp header */
    // SLIP configuration port 42000 on both sides
    udp_hdr->source_port = htons(SPACECRAFT_PORT);
    udp_hdr->dest_port = htons(LANDER_PORT);
    // length
    udp_hdr->len = udpDataSize + sizeof(struct udp_hdr);
    udp_hdr->len = htons(udp_hdr->len);
    udp_hdr->checksum = 0;
    // UDP checksum
    uint16_t udpHeaderChecksum = udp_checksum((uint8_t*) udp_hdr,
                                              udpData,
                                              udp_hdr->len,
                                              ip_hdr->source,
                                              ip_hdr->dest);
    udp_hdr->checksum = htons(udpHeaderChecksum);

     //==============================================================
     // Old code above

    return IP_UDP__STATUS__SUCCESS;
}
