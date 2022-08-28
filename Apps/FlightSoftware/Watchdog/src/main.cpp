#include <stateMachine/RoverStateManager.hpp>
#include <drivers/bsp.h>
#include <msp430.h>


extern "C"
{
    extern unsigned int _stack;

    void hook_sp_check(const char *name)
    {
        uint16_t SP = _get_SP_register();
        if (SP < (uint32_t) &_stack) {
            while (1) {
                __no_operation();
            }
        }
    }
}

int main(void) {
    /* stop watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;

    extern unsigned int __STACK_END;
    unsigned int* p;

    p = &_stack;
    while (p < (&__STACK_END -3))
    {
        *p = 0xA5A5;
        p++;
    }

	const char* resetReason = getResetReasonString();
	iris::RoverStateManager manager(resetReason);
	manager.init();
	manager.spinForever();
}
