/*
 * utils.c
 *
 *  Created on: Mar 12, 2022
 *      Author: Jonathan
 */

/*
 *   A timer that counts clock cycles from 0 to i_timer->Period
 */

#include "include/utils.h"

void iterate_impulse_timer(IMPULSE_TIMER* i_timer, uint8_t driving_open_loop){

    i_timer->cycle = 0x00;      /* Always clear impulse output at entry*/

    if(driving_open_loop){
        i_timer->counter += 2;
    } else {
        i_timer->counter += 1;
    }

    if (i_timer->counter >= i_timer->period){
        i_timer->cycle = 0xff;
        i_timer->counter = 0;        /* Reset counter*/
    }
}

uint8_t iterate_mod6_counter(uint8_t counter, uint8_t driving_open_loop){
    if(driving_open_loop){
        // skip half the motor commutations to speed up driving (sacrifices some torque)
        if(counter >= 4){
            return 0;
        } else {
            return counter + 2;
        }
    } else {
        // kick starting to get to closed loop
        if(counter >= 5){
            return 0;
        } else {
            return counter + 1;
        }
    }
}


