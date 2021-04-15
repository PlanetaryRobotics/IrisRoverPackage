#include "MotorController_i2c.h"

// Timeout for I2C communication
uint16_t i2c_timeout = 1350;

/**
 * @brief      I2c master transmit
 *
 * @param      i2c     Pointer to i2c memory-mapped peripheral
 * @param[in]  sadd    The slave address
 * @param[in]  length  The length
 * @param[in]  data    The data
 *
 * @return     Motor Controller error
 */
void i2cMasterTransmit(i2cBASE_t *i2c, I2cSlaveAddress_t sadd, uint32_t length, uint8_t *data)
{
    // ASSERT i2c not NULL
    // ASSERT data not NULL

    /* Configure address of Slave to talk to */
    i2cSetSlaveAdd(i2c, sadd);
   
    /* Set direction to Transmitter */
    i2cSetDirection(i2c, I2C_TRANSMITTER);
   
    /* Configure Data count */
    i2cSetCount(i2c, length);
   
    /* Set mode as Master */
    i2cSetMode(i2c, I2C_MASTER);
   
    /* Set Stop after programmed Count */
    i2cSetStop(i2c);
   
    /* Transmit Start Condition */
    i2cSetStart(i2c);
   
    /* Transmit DATA_COUNT number of data in Polling mode */
    i2cSend(i2c, length, data);
   
    /* Wait until Bus Busy is cleared */
    uint16_t timeouter = 0;
    while (i2cIsBusBusy(i2c) == true)
    {
      if (++timeouter > i2c_timeout)
        return; //MC_I2C_TIMEOUT_ERROR;
    }   
   
    /* Wait until Stop is detected */
    timeouter = 0;
    while (i2cIsStopDetected(i2c) == 0)
    {
      if (++timeouter > i2c_timeout)
        return; //MC_I2C_TIMEOUT_ERROR;
    }   
   
    /* Clear the Stop condition */
    i2cClearSCD(i2c);
   
    /* Delay long enough for the slave to be ready */
    delayForI2C();
   
    // return MC_NO_ERROR;
}

/**
 * @brief      I2C master receive
 *
 * @param      i2c     I 2 c
 * @param[in]  sadd    The slave address
 * @param[in]  length  The length
 * @param[in]  data    The data
 *
 * @return     Motor controller error
 */
void i2cMasterReceive(i2cBASE_t *i2c, I2cSlaveAddress_t sadd, uint32_t length, uint8_t *data)
{
    // ASSERT i2c not NULL
    // ASSERT data not NULL
   
    /* Configure address of Slave to talk to */
    i2cSetSlaveAdd(i2c, sadd);
   
    /* Set direction to receiver */
    i2cSetDirection(i2c, I2C_RECEIVER);
   
    /* Configure Data count */
    i2cSetCount(i2c, length);
   
    /* Set mode as Master */
    i2cSetMode(i2c, I2C_MASTER);
   
    /* Set Stop after programmed Count */
    i2cSetStop(i2c);
   
    /* Transmit Start Condition */
    i2cSetStart(i2c);
   
    /* Transmit DATA_COUNT number of data in Polling mode */
    i2cReceive(i2c, length, data);
   
    /* Wait until Bus Busy is cleared */
    uint16_t timeouter = 0;
    while (i2cIsBusBusy(i2c) == true)
    {
      if (++timeouter > i2c_timeout)
        return; // MC_I2C_TIMEOUT_ERROR;
    }   
   
    /* Wait until Stop is detected */
    timeouter = 0;
    while (i2cIsStopDetected(i2c) == 0)
    {
      if (++timeouter > i2c_timeout)
        return; //MC_I2C_TIMEOUT_ERROR;
    }
   
    /* Clear the Stop condition */
    i2cClearSCD(i2c);
   
    /* Delay long enough for the slave to be ready */
    delayForI2C();
   
    // return MC_NO_ERROR;
}

/**
* @brief      Delays for 1050 ticks slow enough for slave sides
*
*/
void delayForI2C()
{
    // FIXME: DONT USE POLLING LOOP FOR DELAY
    // for (unsigned i = 180000000; i; --i); ~= 13.5s
    unsigned i;
    for (i = 900; i; --i);
}


