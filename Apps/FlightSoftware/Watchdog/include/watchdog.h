/**
 * @file watchdog.h
 * @brief File header containing function definitions for watchdog
 */
#ifndef __WATCHDOG_INC
#define __WATCHDOG_INC

int watchdog_init();
int watchdog_monitor();
unsigned int watchdog_handle_hercules(unsigned char *buf, uint16_t max_l);

#endif /* __WATCHDOG_INC */


