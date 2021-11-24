#include <stateMachine/RoverStateManager.hpp>
#include <msp430.h>

int main(void) {
    /* stop watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;

//    // TURN ON BATT FUCKING IMMEDIATELY:
//	PJDIR = 0xFFu;
//	PJOUT = 0x00u;
//    PJSEL0 = 0x00u;
//    PJSEL1 = 0x00u;
//    PJOUT |= BIT5;
//    PM5CTL0 &= ~LOCKLPM5;

	iris::RoverStateManager manager;
	manager.init();
	manager.spinForever();
}
