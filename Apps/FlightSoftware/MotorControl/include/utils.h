#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdint.h>

typedef struct { uint16_t  period;		// Input: Period of output impulse in a number of sampling cycles (Q0)
				 uint8_t  cycle;      	// Output: 0x00 or 0xFF indicating if it is time to move to next commutation
				                        //          cycle of motor
		  	  	 uint32_t  counter;   	// Variable: Impulse generator counter (Q0)
			   } IMPULSE_TIMER;


// this macro is used to kick-start closed loop control; doesn't need to be as fast
void iterate_impulse_timer(IMPULSE_TIMER* i_timer, uint8_t driving_open_loop);

uint8_t iterate_mod6_counter(uint8_t counter, uint8_t driving_open_loop);

#endif // __UTILS_H__
