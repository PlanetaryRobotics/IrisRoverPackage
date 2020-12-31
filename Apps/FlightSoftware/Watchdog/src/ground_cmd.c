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
#include "include/uart.h"
#include "include/adc.h"
#include "include/flags.h"

/**
 * Handle watchdog reset-specific hardware commands
 */
void handle_watchdog_reset_cmd(uint8_t cmd) {
    switch(cmd) {
    /* 0x00: No Reset */
    case 0x00:
        break;
    /* 0x01: Reset Hercules */
    case 0x01:
        // TODO
        break;
    /* 0x02: Hercules Power On */
    case 0x02:
        powerOnHercules();
        break;
    /* 0x03: Hercules Power Off */
    case 0x03:
        powerOffHercules();
        break;
    /* 0x04: Reset Radio */
    case 0x04:
        setRadioReset();
        // queue up an radio unreset
        watchdog_flags |= WDFLAG_UNRESET_RADIO1;
        break;
    /* 0x05: Radio Power On */
    case 0x05:
        powerOnRadio();
        break;
    /* 0x06: Radio Power Off */
    case 0x06:
        powerOffRadio();
        break;
    /* 0x07: Reset Camera FPGA */
    case 0x07:
        // TODO is delay cycles really the best way to do things?
        __delay_cycles(1000);
        break;
    /* 0x08: Camera FPGA Power On */
    case 0x08:
        powerOnFpga();
        break;
    /* 0x09: Camera FPGA Power Off */
    case 0x09:
        powerOffFpga();
        break;
    /* 0x0A: Reset Motor 1 */
    case 0x0A:
        // TODO
        break;
    /* 0x0B: Reset Motor 2 */
    case 0x0B:
        // TODO
        break;
    /* 0x0C: Reset Motor 3 */
    case 0x0C:
        // TODO
        break;
    /* 0x0D: Reset Motor 4 */
    case 0x0D:
        // TODO
        break;
    /* 0x0E: Reset All Motors */
    case 0x0E:
        // TODO
        break;
    /* 0x0F: All Motors Power On */
    case 0x0F:
        powerOnMotors();
        break;
    /* 0x10: All Motors Power Off */
    case 0x10:
        powerOffMotors();
        break;
    /* 0x11: Reset 3.3 EN */
    case 0x11:
        // TODO
        break;
    /* 0x12: 3.3 EN On */
    case 0x12:
        enable3V3PowerRail();
        break;
    /* 0x13: 3.3 EN Off */
    case 0x13:
        disable3V3PowerRail();
        break;
    /* 0x14: Reset 24 EN */
    case 0x14:
        // TODO
        break;
    /* 0x15: 24 EN On */
    case 0x15:
        enable24VPowerRail();
        break;
    /* 0x16: 24 EN Off */
    case 0x16:
        disable24VPowerRail();
        break;
    /* 0x17: Deploy */
    case 0x17:
        // only allow this deploy mode if we are not currently attached to the lander
        // TODO: don't use
        if (hasDeployed) setDeploy();
        break;
    /* 0x18: Unset deploy */
    case 0x18:
        // TODO
        break;
    }
}

/**
 * Handle a ground command
 * @param buf: Pointer to the first serialized object byte
 * @param buf_len: Length of the buffer
 */
void handle_ground_cmd(unsigned char *buf, uint16_t buf_len) {
    if (buf_len != 3) {
        /* TODO: this is malformed packet */
        return;
    }
    if (buf[0] != 0xF3) {
        /* TODO: this is malformed packet */
        return;
    }
    if (buf[1] == 0x00) {
        /* watchdog system reset commands */
        handle_watchdog_reset_cmd(buf[2]);
    } else if (buf[1] == 0x01) {
        /* power on all systems */
        enterMode(RS_MISSION);
    } else if (buf[1] == 0x02) {
        /* deploy! */
        if (buf[2] == 0x60 && rovstate == RS_MISSION) {
            setDeploy();
        } else {
            // wrong mode, need to be in
            // TODO: alert end user?
        }
    } else {
        /* TODO: this is malformed packet */
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

/**
 * Send the earth heartbeat
 */
void send_earth_heartbeat() {
    // TODO: UDP/IP headers
    pbuf.used = 0;
    pbuf.idx = 0;

    // build the packet
    pbuf.buf[0] = 0xFF;
    pbuf.buf[1] = (uint8_t)(adc_values[ADC_BATT_LEVEL_IDX] >> 5);
    pbuf.buf[1] = pbuf.buf[1] << 1;
    pbuf.buf[1] |= heaterStatus & 0x1;
    pbuf.buf[2] = (uint8_t)(adc_values[ADC_TEMP_IDX] >> 4);
    pbuf.used += 3;

    uart1_tx_nonblocking(pbuf.buf, pbuf.used);
}


