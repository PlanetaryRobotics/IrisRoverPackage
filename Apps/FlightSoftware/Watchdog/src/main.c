#include <assert.h>
#include <msp430.h>
#include <stdlib.h>
#include <string.h>

#include "include/comms/cmd_msgs.h"
#include "include/comms/hercules_comms.h"
#include "include/comms/ip_udp.h"
#include "include/comms/i2c_sensors.h"
#include "include/comms/lander_comms.h"
#include "include/comms/watchdog_cmd_msgs.h"

#include "include/drivers/uart.h"
#include "include/drivers/bsp.h"
#include "include/drivers/adc.h"

#include "include/flags.h"
#include "include/ground_cmd.h"
#include "include/watchdog.h"

/* define all of the buffers used in other files */
__volatile uint16_t loop_flags;
extern uint8_t heating;
uint8_t lastHeater = 0;
extern uint8_t heatingControlEnabled;

enum rover_state rovstate = RS_KEEPALIVE;

void enterMode(enum rover_state newstate) {
    switch (newstate) {
    case RS_SLEEP:
        /* sleep mode is basically the same thing as keep alive */
    case RS_SERVICE:
        /* service is same as keep alive */
        /* service mode is a temporary step transition to mission mode */
    case RS_KEEPALIVE:
        /* power everything off and set resets */
        powerOffFpga();
        powerOffMotors();
        powerOffRadio();
        powerOffHercules();
        setRadioReset();
        setFPGAReset();
        setMotorsReset();
        setHerculesReset();
        unsetDeploy();

        /* turn off voltage rails */
        disable3V3PowerRail();
        disable24VPowerRail();
        disableBatteries();

        /* monitor only lander voltages */
        adc_setup_lander();
        enableHeater();
        startChargingBatteries();
        break;
    default:
    case RS_MISSION:
        /* bootup process - enable all rails */
        enable3V3PowerRail();
        enable24VPowerRail();
        enableBatteries();
        disableHeater();
        unsetDeploy();

        /* enable hercules uart */
        //!< @todo Reset UART0 to a clean slate.

        /* power everything on and release resets */
        releaseRadioReset();
        releaseFPGAReset();

        /* start monitoring only mission-relevant voltages */
        adc_setup_mission();

        /* power stuff on */
        powerOnFpga();
        powerOnMotors();
        powerOnRadio();
        stopChargingBatteries();

        //!< @todo Get rid of this delay
        __delay_cycles(12345678); //give fuel gauge [50 ms] & wifi [~750 ms] time to start up
        I2C_Sensors__initializeFuelGaugeBlocking();
        powerOnHercules();
        releaseMotorsReset();
        releaseHerculesReset();

        break;
    case RS_FAULT:
        //!< @todo fault mode; enable everything in lander mode
        break;
    }
    rovstate = newstate;
}

//########################################################################################
// Everything below this, down to but excluding main(), is temporary (at least in
// this location, that is). This should all be restructured with switch to high level
// state machine.
// MPS: I'm not documenting these because they will soon be restructured.
//########################################################################################

typedef struct LanderMsgArgContainer
{
    HerculesComms__State* herculesCommsState;
    LanderComms__State* landerCommsState;
} LanderMsgArgContainer;

void sendLanderResponse(LanderComms__State* landerCommsState, WdCmdMsgs__Response* response)
{
    static uint8_t responseSerializationBuffer[WD_CMD_MSGS__PACKED_SIZE__RESPONSE_MSG] = { 0 };

    if (NULL == response) {
        return;
    }

    WdCmdMsgs__Status wdCmdStatus = WdCmdMsgs__serializeGroundResponse(response,
                                                                       responseSerializationBuffer,
                                                                       sizeof(responseSerializationBuffer));

    assert(WD_CMD_MSGS__STATUS__SUCCESS == wdCmdStatus);

    LanderComms__Status lcStatus = LanderComms__txData(landerCommsState,
                                                       responseSerializationBuffer,
                                                       sizeof(responseSerializationBuffer));

    if (LANDER_COMMS__STATUS__SUCCESS != lcStatus) {
        //!< @todo handling?
    }
}

// Determine if ground command for WD or uplink for Hercules, and handle each appropriately
void landerMsgCallback(uint8_t* rxDataBuffer, size_t rxDataLen, void* userArg)
{
    // Statically allocate the message structure so that it's not allocated on the stack
    static WdCmdMsgs__Message wdMessage = { 0 };
    static WdCmdMsgs__Response response = { 0 };
    static WdCmdMsgs__Response deployNotificationResponse = { 0 };

    if (NULL == userArg) {
        return;
    }

    LanderMsgArgContainer* commsPtrs = (LanderMsgArgContainer*) userArg;

    if (NULL == commsPtrs->herculesCommsState || NULL == commsPtrs->landerCommsState) {
        return;
    }

    if (NULL == rxDataBuffer) {
        //!< @todo reply to lander with this error
        return;
    }

    // First of all, parse the FSW common header. We assume it is part of a watchdog command message,
    // as the 
    CmdMsgs__Status cmdStatus = CmdMsgs__deserializeHeader(rxDataBuffer, rxDataLen, &(wdMessage.commonHeader));

    if (CMD_MSGS__STATUS__SUCCESS != cmdStatus) {
        // This should only really happen if rxDataLen is the wrong size
        //!< @todo reply to lander with this error
        return;
    }

    // We identify a ground command for WD by checking the "Type Magic" field in the FSW common header.
    // If the magic number in that field indicates the command is for Watchdog, then it is a ground
    // command for us.
    if (HEADER__TYPE_MAGIC_NUM__WATCHDOG_COMMAND == wdMessage.commonHeader.typeMagicNumber) {
        // This is a watchdog command. Parse it as such
        WdCmdMsgs__Status wdCmdStatus = WdCmdMsgs__deserializeMessage(rxDataBuffer,
                                                                      rxDataLen,
                                                                      &wdMessage,
                                                                      FALSE); // Don't reparse the header

        if (WD_CMD_MSGS__STATUS__SUCCESS != wdCmdStatus) {
            // This should only really happen if rxDataLen is the wrong size
            //!< @todo reply to lander with this error
            return;
        }

        BOOL sendDeployNotificationResponse = FALSE;
        GroundCmd__Status gndCmdStatus = GroundCmd__performWatchdogCommand(&wdMessage,
                                                                           &response,
                                                                           &deployNotificationResponse,
                                                                           &sendDeployNotificationResponse);

        if (GND_CMD__STATUS__SUCCESS != gndCmdStatus) {
            //!< @todo reply to lander with this error
            return;
        }

        if (sendDeployNotificationResponse) {
            sendLanderResponse(commsPtrs->landerCommsState, &deployNotificationResponse);
        }

        sendLanderResponse(commsPtrs->landerCommsState, &response);
    } else {
        // Anything with "Type Magic" field value that isn't for Watchdog is treated as uplink for Hercules
        HerculesComms__Status hcStatus = HerculesComms__txUplinkMsg(commsPtrs->herculesCommsState,
                                                                    rxDataBuffer,
                                                                    rxDataLen);

        if (HERCULES_COMMS__STATUS__SUCCESS != hcStatus) {
            //!< @todo handling? Send response to lander maybe?
        }
    }
}

typedef struct HerculesMsgArgContainer
{
    HerculesComms__State* herculesCommsState;
    LanderComms__State* landerCommsState;
    I2C_Sensors__Readings* i2cReadings;
} HerculesMsgArgContainer;

void handleStrokeFromHercules(HerculesComms__State* herculesCommsState,
                              HercMsgs__Header* header,
                              I2C_Sensors__Readings* i2cReadings)
{
    static uint8_t telemetrySerializationBuffer[16] = { 0 };

    if (NULL == herculesCommsState || NULL == header || NULL == i2cReadings) {
        return;
    }

    // For a stroke we just reply to the Hercules with our telemetry
    watchdog_build_hercules_telem(i2cReadings,
                                  telemetrySerializationBuffer,
                                  sizeof(telemetrySerializationBuffer));


    HerculesComms__Status hcStatus = HerculesComms__txResponseMsg(herculesCommsState,
                                                                  header,
                                                                  telemetrySerializationBuffer,
                                                                  sizeof(telemetrySerializationBuffer));

    if (HERCULES_COMMS__STATUS__SUCCESS != hcStatus) {
        //!< @todo Handle error
    }
}

void handleDownlinkFromHercules(HerculesComms__State* herculesCommsState,
                                LanderComms__State* landerCommsState,
                                HercMsgs__Header* header,
                                uint8_t* payloadBuffer,
                                size_t payloadSize)
{
    if (NULL == herculesCommsState
        || NULL == landerCommsState
        || NULL == header
        || NULL == payloadBuffer
        || 0 == payloadSize) {
        return;
    }

    // For downlink we first send the data to the lander, then we reply to the Hercules

    // 1) Send data to lander
    LanderComms__Status lcStatus = LanderComms__txData(landerCommsState, payloadBuffer, payloadSize);

    if (LANDER_COMMS__STATUS__SUCCESS != lcStatus) {
        //!< @todo Handle error

        // We still want to follow through. Even if we failed to tx the data we can still
        // try to reply to the Hercules
    }
    
    // 2) Reply to Hercules
    HerculesComms__Status hcStatus = HerculesComms__txResponseMsg(herculesCommsState,
                                                                  header,
                                                                  NULL,
                                                                  0);

    if (HERCULES_COMMS__STATUS__SUCCESS != hcStatus) {
        //!< @todo Handle error
    }
}

void handleResetFromHercules(HerculesComms__State* herculesCommsState,
                             HercMsgs__Header* header)
{
    if (NULL == herculesCommsState || NULL == header) {
        return;
    }

    // For Reset_Specific we want to do the reset, then reply to Hercules
    WdCmdMsgs__ResetSpecificId resetValue = (WdCmdMsgs__ResetSpecificId) header->resetValue;
    GroundCmd__Status gcStatus = GroundCmd__performResetCommand(resetValue, NULL);

    if (GND_CMD__STATUS__SUCCESS != gcStatus) {
        //!< @todo logging/handling?

        // We still want to follow through. Even if we failed to do the reset we can still
        // try to reply to the Hercules
    }

    HerculesComms__Status hcStatus = HerculesComms__txResponseMsg(herculesCommsState,
                                                                  header,
                                                                  NULL,
                                                                  0);

    if (HERCULES_COMMS__STATUS__SUCCESS != hcStatus) {
        //!< @todo logging/handling?
    }
}

// Determine if downlink, stroke, or reset command and handle each appropriately
void herculesMsgCallback(HercMsgs__Header* header, uint8_t* payloadBuffer, size_t payloadSize, void* userArg)
{
    if (NULL == userArg) {
        return;
    }

    HerculesMsgArgContainer* args = (HerculesMsgArgContainer*) userArg;

    if (NULL == args->herculesCommsState
        || NULL == args->landerCommsState
        || NULL == args->i2cReadings
        || NULL == header
        || (payloadSize > 0 && NULL == payloadBuffer)) {
        return;
    }
    
    switch (header->lowerOpCode) {
        case HERCULES_COMMS__MSG_OPCODE__STROKE:
            handleStrokeFromHercules(args->herculesCommsState,
                                     header,
                                     args->i2cReadings);
            break;

        case HERCULES_COMMS__MSG_OPCODE__DOWNLINK:
            handleDownlinkFromHercules(args->herculesCommsState,
                                       args->landerCommsState,
                                       header,
                                       payloadBuffer,
                                       payloadSize);
            break;

        default: // We assume this is a reset command
            handleResetFromHercules(args->herculesCommsState, header);
            break;
    }
}

void pumpMsgsFromHercules(I2C_Sensors__Readings* i2cReadings,
                          HerculesComms__State* hCommsState,
                          LanderComms__State* lcState)
{
    HerculesMsgArgContainer args;
    args.herculesCommsState = hCommsState;
    args.landerCommsState = lcState;
    args.i2cReadings = i2cReadings;

    HerculesComms__Status hcStatus = HerculesComms__tryGetMessage(hCommsState,
                                                                  herculesMsgCallback,
                                                                  (void*) &args);

    if (HERCULES_COMMS__STATUS__SUCCESS != hcStatus) {
        //!< @todo logging?
    }
}

void pumpMsgsFromLander(HerculesComms__State* hCommsState,
                        LanderComms__State* lcState)
{
    LanderMsgArgContainer args;
    args.herculesCommsState = hCommsState;
    args.landerCommsState = lcState;

    LanderComms__Status lcStatus = LanderComms__tryGetMessage(lcState,
                                                              landerMsgCallback,
                                                              (void*) &args);

    if (LANDER_COMMS__STATUS__SUCCESS != lcStatus) {
        //!< @todo logging?
    }
}

void sendEarthHeartbeat(I2C_Sensors__Readings* i2cReadings,
                        LanderComms__State* lcState)
{
    static uint8_t hbSerializationBuffer[24] = { 0 };

    size_t outputHeartbeatSize = 0;
    GroundCmd__Status gcStatus = GroundCmd__generateEarthHeartbeat(i2cReadings,
                                                                   hbSerializationBuffer,
                                                                   sizeof(hbSerializationBuffer),
                                                                   &outputHeartbeatSize);

    assert(GND_CMD__STATUS__SUCCESS == gcStatus);
    if (GND_CMD__STATUS__SUCCESS != gcStatus) {
        //!< @todo handling?
        return;
    }

    LanderComms__Status lcStatus = LanderComms__txData(lcState, hbSerializationBuffer, outputHeartbeatSize);

    assert(LANDER_COMMS__STATUS__SUCCESS == lcStatus);
    if (LANDER_COMMS__STATUS__SUCCESS != lcStatus) {
        //!< @todo Handling?
    }
}

int main(void) {
    // Declare the buffers for the UART rx and tx ring buffers. These are static
    // so that they are not on the stack.
    static volatile uint8_t uart0TxBuffer[1024] = { 0 };
    static volatile uint8_t uart0RxBuffer[1024] = { 0 };
    static volatile uint8_t uart1TxBuffer[1024] = { 0 };
    static volatile uint8_t uart1RxBuffer[1024] = { 0 };
    
    /* stop watchdog timer */
	WDTCTL = WDTPW | WDTHOLD;

	/* unlock changes to registers/ports, etc. */
	PM5CTL0 &= ~LOCKLPM5;

	/* initialize the board */
    initializeGpios();

    // Set up UART configuration structures
    UART__Config uartConfig = { 0 };
    uartConfig.uart0Buffers.txBuffer = uart0TxBuffer;
    uartConfig.uart0Buffers.txBufferSize = sizeof(uart0TxBuffer);
    
    uartConfig.uart0Buffers.rxBuffer = uart0RxBuffer;
    uartConfig.uart0Buffers.rxBufferSize = sizeof(uart0RxBuffer);

    uartConfig.uart1Buffers.txBuffer = uart1TxBuffer;
    uartConfig.uart1Buffers.txBufferSize = sizeof(uart1TxBuffer);
    
    uartConfig.uart1Buffers.rxBuffer = uart1RxBuffer;
    uartConfig.uart1Buffers.rxBufferSize = sizeof(uart1RxBuffer);

    // Initialize the UARTs (this one call initializes both).
    // This call also initializes the clocks as part of the UART initialization.
    UART__State* uart0State = NULL;
    UART__State* uart1State = NULL;
    UART__Status uartStatus = UART__init(&uartConfig,
                                         &uart0State,
                                         &uart1State);
    assert(UART__STATUS__SUCCESS == uartStatus);

    // Set up the comms modules
    HerculesComms__State* hcState;
    HerculesComms__Status hcStatus = HerculesComms__init(&hcState, uart0State);
    assert(HERCULES_COMMS__STATUS__SUCCESS == hcStatus);

    LanderComms__State* lcState;
    LanderComms__Status lcStatus = LanderComms__init(&lcState, uart1State);
    assert(LANDER_COMMS__STATUS__SUCCESS == lcStatus);

    /* set up watchdog */
    watchdog_init();

    /* set up the ADC */
    adc_init();

    /* set up i2c to read from fuel gauge*/
    I2C_Sensors__init();

    /* enter keepalive mode */
    enterMode(rovstate);

    __bis_SR_register(GIE); // Enable all interrupts

    char helloWorld[15] = "hello, world!\r\n";
    lcStatus = LanderComms__txData(lcState, (uint8_t*) helloWorld, sizeof(helloWorld));
    assert(LANDER_COMMS__STATUS__SUCCESS == lcStatus);

    I2C_Sensors__Readings i2cReadings = { 0 }; // will hold i2c data

    uint8_t keepAliveTimerTickCounter = 0;

    // the core structure of this program is like an event loop
    while (1) {
        /* watchdog timer setup - need to stroke every ~1s */
        /* basically, we limit the execution of each loop to ~1s or else reset */
        WDTCTL = (WDTPW+WDTCNTCL+WDTSSEL0+WDTIS_3); // ~ 16 seconds

        /* check if anything happened */
        if (!loop_flags) { /* nothing did */
            /* go back to low power mode */
            __bis_SR_register(GIE);
            continue;
        }

        /* Got data from hercules */
        if (loop_flags & FLAG_UART0_RX_PACKET) {
            pumpMsgsFromHercules(&i2cReadings, hcState, lcState);

            /* clear event when done */
            loop_flags ^= FLAG_UART0_RX_PACKET;
        }

        /* Got data from lander */
        if (loop_flags & FLAG_UART1_RX_PACKET) {
            pumpMsgsFromLander(hcState, lcState);

            /* clear event when done */
            loop_flags ^= FLAG_UART1_RX_PACKET;
        }

        if (loop_flags & FLAG_TIMER_TICK) {
            /* handle event for heartbeat */
            /* always sample the ADC for temperature and voltage levels */
            adc_sample();

            switch (rovstate) {
                case RS_SERVICE:
                    sendEarthHeartbeat(&i2cReadings, lcState);

                    if (heatingControlEnabled) {
                        heaterControl();
                    }

                    watchdog_monitor(hcState);
                    break;

                case RS_KEEPALIVE:
                    keepAliveTimerTickCounter++;

                    /* only send every 3 timer ticks (15s) */
                    if (keepAliveTimerTickCounter >= 3) {
                        keepAliveTimerTickCounter = 0;

                        /* send heartbeat with collected data */
                        sendEarthHeartbeat(&i2cReadings, lcState);
                    }

                    if (heatingControlEnabled) {
                        // calculate PWM duty cycle (if any) to apply to heater
                        heaterControl();
                    }
                    break;

                case RS_MISSION:
                    /* check for kicks from devices and reset misbehaving things */
                    
                    /**
                     * @todo Check if we've deployed or if we shouldn't be able to communicate with lander for any other
                     *       reason, and don't send this if that is the case.
                     */
                    sendEarthHeartbeat(&i2cReadings, lcState);

                    watchdog_monitor(hcState);

                    // Initiate gauge readings here, the rest of the actions to do in this state every state
                    // will be done after gauge readings complete, which is monitored in the
                    // FLAG_I2C_GAUGE_READING_ACTIVE loop_flags block below.
                    I2C_Sensors__initiateGaugeReadings();
                    loop_flags |= FLAG_I2C_GAUGE_READING_ACTIVE;
                    break;

                case RS_FAULT:
                    /* sad :( */
                    break;
            }

            /* clear event when done */
            loop_flags ^= FLAG_TIMER_TICK;
        }

        if (loop_flags & FLAG_I2C_GAUGE_READING_ACTIVE) {
            I2C_Sensors__spinOnce();

            I2C_Sensors__Status stat = I2C_Sensors__getGaugeReadingStatus(&i2cReadings);

            int done = (I2C_SENSORS__STATUS__INCOMPLETE != stat);

            if (done) {
                loop_flags ^= FLAG_I2C_GAUGE_READING_ACTIVE;
            }
        }

        if (lastHeater ^ heating) {
            if (heating) {
                enableHeater();
            } else {
                disableHeater();
            }
        }

        lastHeater = heating;

        if (loop_flags & FLAG_TEMP_HIGH) {
            if (rovstate == RS_KEEPALIVE)
                /* it only makes sense to disable the heaters if we're connected to the lander */
                disableHeater();

            /* clear event when done */
            loop_flags ^= FLAG_TEMP_HIGH;
        }

        if (loop_flags & FLAG_POWER_ISSUE) {
            if (rovstate == RS_MISSION) {
                //!< @todo Turn off various power lines & enter fault mode.
            }
            /* clear event when done */
            loop_flags ^= FLAG_POWER_ISSUE;
        }
    }
}
