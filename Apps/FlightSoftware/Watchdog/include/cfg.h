/*
 * cfg.h
 *
 *  Created on: May 24, 2021
 *      Author: xprize
 */

#ifndef INCLUDE_CFG_H_
#define INCLUDE_CFG_H_


// SLIP configuration spacecraft address
// 192.168.103.1 => 0xC0A86701 => big endian'd
#define LANDER_ADDRESS 0x0167A8C0
//0x0167A8C0

// SLIP configuration payload address
// 192.168.103.2 => 0xC0A86702 => big endian'd
#define SPACECRAFT_ADDRESS 0x0267A8C0
//0x0267A8C0

#define LANDER_PORT 42000
#define SPACECRAFT_PORT 42000

#endif /* INCLUDE_CFG_H_ */
