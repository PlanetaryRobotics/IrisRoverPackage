#ifndef __IMPULSE_H__
#define __IMPULSE_H__

typedef struct { uint16_t  Period;		// Input: Period of output impulse in a number of sampling cycles (Q0)
				 uint8_t  Out;      		// Output: Impulse generator output - Q0 (0x00000000 or 0x00007FFF)
		  	  	 uint16_t  Counter;   	// Variable: Impulse generator counter (Q0)
			   } IMPULSE;	            

/*------------------------------------------------------------------------------
	IMPULSE Macro Definition
------------------------------------------------------------------------------*/

#define IMPULSE_MACRO(v)										\
  																\
  v.Out = 0x00;      /* Always clear impulse output at entry*/ \
  v.Counter++;    /* Increment the skip counter*/				\
																\
  if (v.Counter >= v.Period)									\
  {																\
     v.Out = 0xff;										        \
     v.Counter = 0;        /* Reset counter*/					\
  } 

#define OPEN_LOOP_IMPULSE_MACRO(v)                                        \
                                                                \
  v.Out = 0x00;      /* Always clear impulse output at entry*/ \
  v.Counter+=2;    /* Increment the skip counter*/               \
                                                                \
  if (v.Counter >= v.Period)                                    \
  {                                                             \
     v.Out = 0xff;                                              \
     v.Counter = 0;        /* Reset counter*/                   \
  }

#endif // __IMPULSE_H__
