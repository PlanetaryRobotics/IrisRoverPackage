/*
 * ground_cmd.c
 *
 * Handle commands sent from the ground up to the watchdog
 *
 *  Created on: Nov. 29, 2020
 *      Author: ohnx
 */
#include <stdint.h>
#include "include/bsp.h"
#include "include/buffer.h"
#include "include/ip_udp.h"

/**
 * Handle a ground command
 * @param buf: Pointer to the first serialized object byte
 * @param buf_len: Length of the buffer
 */
void handle_ground_cmd(unsigned char *buf, uint16_t buf_len) {
    if (buf_len < 2) {
        /* TODO: this is malformed packet */
        return;
    }
    if (buf[0] == 0xF3 && buf[1] == 0x00) {
        /* watchdog system reset */
        if (buf_len != 3) {
            /* TODO: this is malformed packet */
            return;
        }

        if (buf[2] & 0x08) {
            /* reset FPGA */
            setRadioReset();
            setFPGAReset();
            setMotorsReset();
            setHerculesReset();
            powerOffHercules();
            powerOffFpga();
            powerOffMotors();
            powerOffRadio();
            disable24VPowerRail();
            disable3V3PowerRail();
        }
    }
}

/**
 * Parse a ground command
 *
 * Will
 *
 * @param pp: Buffer containing the parsed packet. idx = index to start parsing at
 * and used = maximum value of idx
 */
void parse_ground_cmd(struct buffer *pp) {
    unsigned char *buf;
    uint16_t pp_len, packet_len;

    /* parse the ip and udp headers */
    buf = ipudp_parse_packet(pp, &pp_len);
    if (!buf) {
        /* malformed packet */
        return;
    }

    /* check that the length is greater than the base sane amount */
    if (pp_len < 9) {
        /* TODO: return some sort of malformed packet warning? */
        return;
    }

    /* first is sequence number */
    /* TODO: sequence number */
    /* next is two bytes, big endian, representing length of the packet */
    packet_len = buf[1] << 8 | buf[2];
    /* check that the length is right */
    if (packet_len != pp_len) {
        /* TODO: what is the packet_len field actually representing? */
    }

    /* TODO: check the checksum */
    /* get the type */
    if (buf[5] == 0x00 && buf[6] == 0xBA && buf[7] == 0xDA && buf[8] == 0x55) {
        /* command */
        handle_ground_cmd(buf + 9, pp_len - 9);
    }
}


