#ifndef __MOD6_CNT_H__
#define __MOD6_CNT_H__

typedef struct { uint8_t  Counter;	    // Output: Modulo 6 counter output - Q0 (0,1,2,3,4,5)
                 int8_t Direction;      // 1 positive, -1 negative
			   } MOD6CNT;

/*------------------------------------------------------------------------------
	MOD6_CNT Macro Definition
------------------------------------------------------------------------------*/


#define MOD6CNT_MACRO(v)											\
 if (v.Counter >= 5)    /* Reset the counter when it is 5 */		\
   v.Counter = 0;													\
 else																\
   v.Counter += v.Direction;         /* Otherwise, increment by 1 */\

#endif // __MOD_6CNT_H__ 
