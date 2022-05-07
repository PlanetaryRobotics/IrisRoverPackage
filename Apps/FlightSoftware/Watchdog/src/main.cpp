#include <stateMachine/RoverStateManager.hpp>
#include <drivers/bsp.h>
#include <msp430.h>

int main(void) {
    /* stop watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;

	const char* resetReason = getResetReasonString();
	iris::RoverStateManager manager(resetReason);
	manager.init();
	manager.spinForever();
}
