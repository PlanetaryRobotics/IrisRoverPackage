#include "MotorController_i2c.h"

#include <FreeRTOSConfig.h>

// Timeout for I2C communication
const uint16_t i2c_timeout = 51350;

/*
 * Reimplementation of HAL's i2cSend (without interrupt mode) which times out if nothing was
 * received in a certain amount of cycles. Returns false if timed out, true on success
 */
static bool i2cSendWithTimeout(i2cBASE_t *i2c, uint32 length, uint8 * data, unsigned timeout) {
    while (length > 0U)
    {
        /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
        while ((i2c->STR & (uint32)I2C_TX_INT) == 0U)
        {
            if (!timeout)
                // return false;
            timeout--;
        } /* Wait */
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        i2c->DXR = (uint32)(*data);
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        data++;
        length--;
    }
    return true;
}

/*
 * Reimplementation of HAL's i2cReceive (without interrupt mode) which times out if nothing was
 * received in a certain amount of cycles. Returns false if timed out, true on success
 */
static bool i2cReceiveWithTimeout(i2cBASE_t *i2c, uint32 length, uint8 * data, unsigned timeout) {
    while (length > 0U)
    {
        /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
        while ((i2c->STR & (uint32)I2C_RX_INT) == 0U)
        {
            if (!timeout)
                // return false;
            timeout--;
        } /* Wait */
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        *data = ((uint8)i2c->DRR);
        /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
        /*SAFETYMCUSW 567 S MR:17.1,17.4 <APPROVED> "Pointer increment needed" */
        data++;
        length--;
    }
    return true;
}

bool i2cMasterTransmit(i2cBASE_t *i2c, I2cSlaveAddress_t sadd, uint32_t length, uint8_t *data) {
    // ASSERT i2c not NULL
    // ASSERT data not NULL

    i2cSetSlaveAdd(i2c, sadd);
    i2cSetDirection(i2c, I2C_TRANSMITTER);
    i2cSetCount(i2c, length);
    i2cSetMode(i2c, I2C_MASTER);
    i2cSetStop(i2c);
    i2cSetStart(i2c);

    // TODO: configCPU_CLOCK_HZ use this to compute optimal timeout
    // (20 lines of code * 4 instr/line) / (16e6 Hz) * 110e6Hz => 550 cycles <= number of Hercules cycles for MSP to be ready
//    int maxReceiveAttempts = 4;
//    while (maxReceiveAttempts) {
//        if (i2cSendWithTimeout(i2c, length, data, 600))
//            break;
//        maxReceiveAttempts--;
//    }
//    if (!maxReceiveAttempts)
//        return false;
    i2cSend(i2c, length, data);
   
    int timeout = 0;
    while (i2cIsBusBusy(i2c)) {
//      if (++timeout > i2c_timeout)
//        return false;
    }

    timeout = 0;
    //while (i2cIsStopDetected(i2c)) {
//      if (++timeout > i2c_timeout)
//        return false;
    //}

    timeout = 0;
    while (i2cIsMasterReady(i2c)) {
//      if (++timeout > i2c_timeout)
//        return false;
    }
   
    i2cClearSCD(i2c);   // Clear the Stop condition
   
    /* Delay long enough for the slave to be ready */
    delayForI2C();

    return true;
}

bool i2cMasterReceive(i2cBASE_t *i2c, I2cSlaveAddress_t sadd, uint32_t length, uint8_t *data) {
    // ASSERT i2c not NULL
    // ASSERT data not NULL
    
    i2cSetSlaveAdd(i2c, sadd);
    i2cSetDirection(i2c, I2C_RECEIVER);
    i2cSetCount(i2c, length);
    i2cSetMode(i2c, I2C_MASTER);
    i2cSetStop(i2c);
    i2cSetStart(i2c);
    
    // TODO: configCPU_CLOCK_HZ use this to compute optimal timeout
    // (20 lines of code * 4 instr/line) / (16e6 Hz) * 110e6Hz => 550 cycles <= number of Hercules cycles for MSP to be ready
//    int maxReceiveAttempts = 4;
//    while (maxReceiveAttempts) {
//        if (i2cReceiveWithTimeout(i2c, length, data, 600))
//            break;
//        maxReceiveAttempts--;
//    }
//    if (!maxReceiveAttempts)
//        return false;
    i2cReceive(i2c, length, data);

    int timeout = 0;
    while (i2cIsBusBusy(i2c)) {
//      if (++timeout > i2c_timeout)
//        return false;
    }

    timeout = 0;
//     while (i2cIsStopDetected(i2c)) {
//      if (++timeout > i2c_timeout)
//        return false;
//     }

    timeout = 0;
    while (i2cIsMasterReady(i2c)) {
//      if (++timeout > i2c_timeout)
//        return false;
    }
   
    i2cClearSCD(i2c);  // Clear the Stop condition
   
    /* Delay long enough for the slave to be ready */
    delayForI2C();

    return true;
}

/**
* @brief      Delays for 1050 ticks slow enough for slave sides
*
*/
void delayForI2C() {
    // FIXME: DONT USE POLLING LOOP FOR DELAY
    // for (unsigned i = 180000000; i; --i); ~= 13.5s
    unsigned delay = 5500000;
        while (delay)    // Delay 0.5s to give the motors a chance to converge. 0.5 / (1/ 110e6)
            delay--;
}


