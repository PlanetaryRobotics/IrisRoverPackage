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
#include "include/i2c.h"

void enterMode(enum rover_state newstate);

/**
 * Handle watchdog reset-specific hardware commands
 */
uint8_t handle_watchdog_reset_cmd(uint8_t cmd) {
    switch(cmd) {
    /* 0x00: No Reset */
    case 0x00:
        break;
    /* 0x01: Reset Hercules */
    case 0x01:
        setHerculesReset();
        // queue up hercules unreset
        watchdog_flags |= WDFLAG_UNRESET_HERCULES;
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
        setFPGAReset();
        // queue up the fpga unreset
        watchdog_flags |= WDFLAG_UNRESET_FPGA;
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
        setMotor1Reset();
        // queue up the motor 1 unreset
        watchdog_flags |= WDFLAG_UNRESET_MOTOR1;
        break;
    /* 0x0B: Reset Motor 2 */
    case 0x0B:
        setMotor2Reset();
        // queue up the motor 2 unreset
        watchdog_flags |= WDFLAG_UNRESET_MOTOR2;
        break;
    /* 0x0C: Reset Motor 3 */
    case 0x0C:
        setMotor3Reset();
        // queue up the motor 3 unreset
        watchdog_flags |= WDFLAG_UNRESET_MOTOR3;
        break;
    /* 0x0D: Reset Motor 4 */
    case 0x0D:
        setMotor4Reset();
        // queue up the motor 4 unreset
        watchdog_flags |= WDFLAG_UNRESET_MOTOR4;
        break;
    /* 0x0E: Reset All Motors */
    case 0x0E:
        setMotorsReset();
        // queue up all the motor unresets
        watchdog_flags |= WDFLAG_UNRESET_MOTOR1 | WDFLAG_UNRESET_MOTOR2 |
                          WDFLAG_UNRESET_MOTOR3 | WDFLAG_UNRESET_MOTOR4;
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
        disable3V3PowerRail();
        // queue up 3V3 rail on again
        watchdog_flags |= WDFLAG_UNRESET_3V3;
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
        disable24VPowerRail();
        // queue up 24V rail on again
        watchdog_flags |= WDFLAG_UNRESET_24V;
        break;
    /* 0x15: 24 EN On */
    case 0x15:
        enable24VPowerRail();
        break;
    /* 0x16: 24 EN Off */
    case 0x16:
        disable24VPowerRail();
        break;
    /* 0xEE: Deploy */
    case 0xEE:
        /* WOOT WOOT! WE ARE ON TO THE MOON, FOLKS */
        /* ref: https://docs.google.com/document/d/1dKLlBcIIVo8t1bGu3jNiHobGMavA3I2al0cncj3ZAhE/edit */
        setDeploy();
        break;
    /* 0x18: Unset deploy */
    case 0x18:
        unsetDeploy();
        break;
    /* 0x19: FPGA camera select #0 */
    case 0x19:
        fpgaCameraSelectLo();
        break;
    /* 0x20: FPGA camera select #1 */
    case 0x20:
        fpgaCameraSelectHi();
        break;
    default:
        /* invalid command */
        return 0xff;
    }
    /* all ok */
    return 0;
}


#define GNDRESP_ENONE        0
#define GNDRESP_EPACKETLEN   1
#define GNDRESP_ECHKSM       2
#define GNDRESP_EMODID       3
#define GNDRESP_ECMDID       4
#define GNDRESP_ECMDPARAM    5
#define GNDRESP_ECMDSEQ      6

/**
 * Reply to a ground command
 *
 * @param cmdid: Command ID replying to (may be 0 if entire packet invalid)
 * @param error_no: Error GNDRESP_E*
 */
void reply_ground_cmd(uint8_t cmdid, uint8_t error_no) {
    uint8_t send_buf[3];

    // fill the buffer
    send_buf[0] = 0x0A;
    send_buf[1] = cmdid;
    send_buf[2] = error_no;

    // send the bytes
    ipudp_send_packet(send_buf, 3);
}

/**
 * Handle a ground command
 * @param buf: Pointer to the first serialized object byte
 * @param buf_len: Length of the buffer
 */
void handle_ground_cmd(unsigned char *buf, uint16_t buf_len) {
    if (buf_len <= 3) {
        /* invalid packet length */
        reply_ground_cmd(0, GNDRESP_EPACKETLEN);
        return;
    }
    if (buf[1] != 0x10) {
        /* module ID invalid */
        reply_ground_cmd(0, GNDRESP_EMODID);
        return;
    }
    switch (buf[0]) {
    case 0x00:
        /* watchdog system reset commands */
        if (handle_watchdog_reset_cmd(buf[2])) {
            /* invalid argument */
            reply_ground_cmd(0, GNDRESP_ECMDPARAM);
        }
        break;
    case 0x01:
        /* power on all systems */
        if (buf[2] != 0x60) {
            /* magic bad */
            reply_ground_cmd(0, GNDRESP_ECMDPARAM);
            return;
        } else if (rovstate != RS_SERVICE) {
            /* not in the right state to transition to mission mode */
            reply_ground_cmd(0, GNDRESP_ECMDSEQ);
            return;
        }
        /* all checks pass, enter mission mode */
        enterMode(RS_MISSION);
        break;
    case 0x02:
    case 0x04:
        /* TODO: not actually used */
        break;
    case 0xAC:
        /* TODO: set thermistor Kp */
        break;
    case 0xBC:
        /* TODO: set thermistor Ki */
        break;
    case 0xCC:
        /* TODO: set thermistor Kd */
        break;
    case 0xDA:
        /* TODO: set thermistor V setpoint */
        break;
    case 0xEA:
        /* Enter sleep mode */
        if (buf[2] != 0x77) {
            /* magic bad */
            reply_ground_cmd(0, GNDRESP_ECMDPARAM);
            return;
        } else if (rovstate != RS_KEEPALIVE) {
            /* not in the right state to transition to mission mode */
            reply_ground_cmd(0, GNDRESP_ECMDSEQ);
            return;
        }
        /* all checks pass, enter sleep mode */
        enterMode(RS_SLEEP);
        break;
    case 0xEB:
        /* Enter keepalive mode */
        if (buf[2] != 0x77) {
            /* magic bad */
            reply_ground_cmd(0, GNDRESP_ECMDPARAM);
            return;
        } else if (rovstate != RS_SLEEP || rovstate != RS_SERVICE) {
            /* not in the right state to transition to keepalive mode */
            reply_ground_cmd(0, GNDRESP_ECMDSEQ);
            return;
        }
        /* all checks pass, enter keepalive mode */
        enterMode(RS_KEEPALIVE);
        break;
    case 0xEC:
        /* Enter service mode */
        if (buf[2] != 0x77) {
            /* magic bad */
            reply_ground_cmd(0, GNDRESP_ECMDPARAM);
            return;
        } else if (rovstate != RS_KEEPALIVE) {
            /* not in the right state to transition to service mode */
            reply_ground_cmd(0, GNDRESP_ECMDSEQ);
            return;
        }
        /* all checks pass, enter mission mode */
        enterMode(RS_SERVICE);
        break;
    default:
        /* invalid command */
        reply_ground_cmd(0, GNDRESP_ECMDID);
        break;
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
        reply_ground_cmd(0, GNDRESP_EPACKETLEN);
        return;
    }

    /* check that the length is greater than the base sane amount */
    if (pp_len < 11) {
        /* malformed packet length */
        reply_ground_cmd(0, GNDRESP_EPACKETLEN);
        return;
    }

    /* first is sequence number */
    /* TODO: sequence number */
    /* next is two bytes, little endian, representing length of the packet */
    packet_len = buf[2] << 8 | buf[1];
    /* check that the length is right */
    if ((packet_len + 4) != pp_len) {
        /* malformed packet length */
        reply_ground_cmd(0, GNDRESP_EPACKETLEN);
        return;
    }

    /* TODO: check the checksum */
    (void)(buf[3]);

    /* get the type (should be 000BADA55 for hercules) */
    if (buf[4] == 0x55 && buf[5] == 0xDA && buf[6] == 0xBA && buf[7] == 0x00) {
        /* command */
        handle_ground_cmd(buf + 8, pp_len - 8);
    } else {
        /* forward it on to hercules */
        // copy the buffer into the hercules buffer
        memcpy(hercbuf.buf + hercbuf.idx, pp->buf, pp->used);
        hercbuf.idx += pp->used;
    }

    /* done parsing */
    // TODO: multiple packets? handle.
    pp->used = 0;
    pp->idx = 0;
}

/**
 * Send the earth heartbeat
 */
void send_earth_heartbeat() {
    static uint8_t counter = 0;
    if (counter % 3 != 2) {
        // send every 2 seconds
        counter++;
        return;
    }
    counter = 0;

    pbuf.used = 0;
    pbuf.idx = 0;

    // build the packet
    pbuf.buf[0] = 0xFF;
    // send the battery voltage
    pbuf.buf[1] = (uint8_t)(raw_battery_voltage[0] >> 1); //TODO: fix this
    pbuf.buf[1] = pbuf.buf[1] << 1;
    // send heater on status
    pbuf.buf[1] |= heaterStatus & 0x1;
    // battery current
    pbuf.buf[2] = (uint8_t)(raw_battery_current[0] >> 1); //TODO: fix this
    // send voltage nominal status
    pbuf.buf[2] |= 0x1; // TODO: fix this

    // send the thermistor temperature (12 bits to 8 bits)
    pbuf.buf[3] = (uint8_t)(adc_values[ADC_TEMP_IDX] >> 4);
    pbuf.used += 4;

    // send the packet!
    ipudp_send_packet(pbuf.buf, pbuf.used);
}


