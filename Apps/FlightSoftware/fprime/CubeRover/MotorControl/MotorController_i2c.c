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

bool i2cMasterReadData(i2cBASE_t *i2c, I2cSlaveAddress_t sadd, uint8_t slaveRegToReadAddr, uint32_t length, uint8_t *data) {
    // ASSERT i2c not NULL
    // ASSERT data not NULL

    /* wait until MST bit gets cleared, this takes
     * few cycles after Bus Busy is cleared */
    while(i2cIsMasterReady(i2c) != true);

    /* Configure address of Slave to talk to */
    i2cSetSlaveAdd(i2c, sadd);

    /* Set direction to Transmitter */
    /* Note: Optional - It is done in Init */
    i2cSetDirection(i2c, I2C_TRANSMITTER);

    /* Configure Data count */
    /* Slave address + Word address write operation before reading */
    i2cSetCount(i2c, 1);

    /* Set mode as Master */
    i2cSetMode(i2c, I2C_MASTER);

    /* Set Stop after programmed Count */
    i2cSetStop(i2c);

    /* Transmit Start Condition */
    i2cSetStart(i2c);

    /* Send the Word Address */
    i2cSendByte(i2c, slaveRegToReadAddr);

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(i2c) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(i2c) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(i2c);

    /*****************************************/
    //// Start receving the data From Slave
    /*****************************************/

    /* wait until MST bit gets cleared, this takes
     * few cycles after Bus Busy is cleared */
    while(i2cIsMasterReady(i2c) != true);

    /* Configure address of Slave to talk to */
    i2cSetSlaveAdd(i2c, sadd);

    /* Set direction to receiver */
    i2cSetDirection(i2c, I2C_RECEIVER);

    /* Configure Data count */
    /* Note: Optional - It is done in Init, unless user want to change */
    i2cSetCount(i2c, length);

    /* Set mode as Master */
    i2cSetMode(i2c, I2C_MASTER);

    /* Set Stop after programmed Count */
    i2cSetStop(i2c);

    /* Transmit Start Condition */
    i2cSetStart(i2c);

    /* Receive DATA_COUNT number of data in Polling mode */
    i2cReceive(i2c, length, data);

    /* Wait until Bus Busy is cleared */
    while(i2cIsBusBusy(i2c) == true);

    /* Wait until Stop is detected */
    while(i2cIsStopDetected(i2c) == 0);

    /* Clear the Stop condition */
    i2cClearSCD(i2c);

    return true;
}

// THE OTHER NEW i2c function
bool i2cMasterTransmit2(i2cBASE_t *i2c, I2cSlaveAddress_t sadd, uint8_t slaveRegToWriteAddr, uint32_t length, uint8_t *data) {
    // ASSERT i2c not NULL
    // ASSERT data not NULL

    /* wait until MST bit gets cleared, this takes
     * few cycles after Bus Busy is cleared */
    while(i2cIsMasterReady(i2c) != true);

    i2cSetSlaveAdd(i2c, sadd);
    i2cSetDirection(i2c, I2C_TRANSMITTER);
    i2cSetCount(i2c, length);
    i2cSetMode(i2c, I2C_MASTER);
    i2cSetStop(i2c);
    i2cSetStart(i2c);

    /* Send the Word Address */
    i2cSendByte(i2c, slaveRegToWriteAddr);

    i2cSend(i2c, length, data);

    int timeout = 0;
    while (i2cIsBusBusy(i2c)) {
//      if (++timeout > i2c_timeout)
//        return false;
    }

    timeout = 0;
    while (i2cIsStopDetected(i2c)) {
//      if (++timeout > i2c_timeout)
//        return false;
    }

    i2cClearSCD(i2c);   // Clear the Stop condition

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

//    timeout = 0;
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

/**
* @brief      Delays for 1050 ticks slow enough for slave sides
*
*/
void delayForI2C() {
    // FIXME: DONT USE POLLING LOOP FOR DELAY
    // for (unsigned i = 180000000; i; --i); ~= 13.5s
    unsigned i;
    for (i = 100000; i; --i) {
        volatile j = 5;
        j++;
    }
}


