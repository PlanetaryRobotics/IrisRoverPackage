#include <stdint.h>
#include "ip_udp.h"

/* note - msp430 is little endian; networks are big endian */

/**
 * Return the IP checksum of the blob given, **in host byte order**
 */
uint16_t ip_checksum(uint8_t *packet, uint16_t packet_len) {
    uint16_t packet_len_orig;
    uint16_t i;
    uint16_t tmp;
    uint32_t accum = 0xffff;
    
    /* store the original packet length before we modify it */
    packet_len_orig = packet_len;
    if (packet_len % 1) {
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
        tmp = (packet[i] << 8);

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
uint16_t ip_verify_packet(uint8_t *packet, uint16_t packet_len) {
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

    /* TODO: double-check the ip addresses are correct */

    /* verify the checksum */
    word_stor = ip_checksum(packet, sizeof(struct ip_hdr));

    /* ip checksum should be 0 or 0xffff (negative 0) for a valid packet */
    return (word_stor == 0xffff || word_stor == 0x0) ? 0 : word_stor;
}


/* to compile: gcc ip_udp.c -Wall -Wxtra -Werror -D__TEST_IP_UDP -o test_ip_udp */
#ifdef __TEST_IP_UDP
#include <stdio.h>
#include <assert.h>

int main() {
    uint8_t goog_query_bytes[] = {
        0x45, 0x00,
        0x00, 0x38, 0x00, 0x00, 0x40, 0x00, 0x40, 0x11,
        0x65, 0x47, 0xc0, 0xa8, 0xaa, 0x08, 0xc0, 0xa8,
        0xaa, 0x14, 0x80, 0x1b, 0x00, 0x35, 0x00, 0x24,
        0x85, 0xed, 0x10, 0x32, 0x01, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x67,
        0x6f, 0x6f, 0x67, 0x6c, 0x65, 0x03, 0x63, 0x6f,
        0x6d, 0x00, 0x00, 0x10, 0x00, 0x01
    };
    uint8_t example_query_bytes[] = {
        0x45, 0x00,
        0x00, 0x44, 0xc4, 0x16, 0x00, 0x00, 0x40, 0x11,
        0x00, 0x00, 0xc0, 0xa8, 0x01, 0x9d, 0x01, 0x01,
        0x01, 0x01, 0xcd, 0xab, 0x00, 0x35, 0x00, 0x30,
        0x20, 0x60, 0x6d, 0x4e, 0x01, 0x20, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0x65,
        0x78, 0x61, 0x6d, 0x70, 0x6c, 0x65, 0x03, 0x63,
        0x6f, 0x6d, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00,
        0x00, 0x29, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00
    };
    uint16_t my_checksum;

    printf("sizeof(ip_hdr) = %lu, ", sizeof(struct ip_hdr));
    printf("sizeof(ip_pseudohdr) = %lu, ", sizeof(struct ip_pseudohdr));
    printf("sizeof(udp_hdr) = %lu, ", sizeof(struct udp_hdr));
    printf("sizeof(checksum_udp_pckt) = %lu, ", sizeof(struct checksum_udp_pckt));
    printf("sizeof(ip_udp_pckt) = %lu\n", sizeof(struct ip_udp_pckt));

    assert(sizeof(example_query_bytes) == 68);
    my_checksum = ip_verify_packet(example_query_bytes, sizeof(example_query_bytes));
    assert(0xf24b == my_checksum);

    assert(sizeof(goog_query_bytes) == 56);
    my_checksum = ip_verify_packet(goog_query_bytes, sizeof(goog_query_bytes));
    assert(0x0 == my_checksum);

    return 0;
}

#endif




