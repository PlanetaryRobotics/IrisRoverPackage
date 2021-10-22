#include <stateMachine/RoverStateManager.hpp>
#include <msp430.h>

int main(void) {
    /* stop watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;

	iris::RoverStateManager manager;
	manager.init();
	manager.spinForever();
}
