#include "MotorController_i2c.h"

#include <FreeRTOSConfig.h>

// Timeout for I2C communication
const uint16_t i2c_timeout = 51350;

/*
 * Reimplementation of HAL's i2cSend (without interrupt mode) which times out if nothing was
 * received in a certain amount of cycles. Returns false if timed out, true on success
 */
static bool i2cSendWithTimeout(i2cBASE_t *i2c, uint32 length, uint8 *data, unsigned timeout)
{
    while (length > 0U)
    {
        /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
        while ((i2c->STR & (uint32)I2C_TX_INT) == 0U)
        {
            if (!timeout)
                return false; // exits early and returns fail if timeout counter hits 0
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
static bool i2cReceiveWithTimeout(i2cBASE_t *i2c, uint32 length, uint8 *data, unsigned timeout)
{
    while (length > 0U)
    {
        /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Potentially infinite loop found - Hardware Status check for execution sequence" */
        while ((i2c->STR & (uint32)I2C_RX_INT) == 0U)
        {
            if (!timeout)
                return false; // exits early and returns fail if timeout counter hits 0
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

static bool waitWhileDeviceIsBusy(i2cBASE_t *i2c, I2cSlaveAddress_t slaveAddress)
{
    // Quits the operation if the cumulative number of cycles spent waiting across all operations is greater than cumulative_timeout_cycles:
    uint32_t cumulative_timeout_cycles = I2C_MASTER_READ_WRITE_MAX_DELAY_CYCLES;

    // Wait until the bus is not busy
    while (i2c->STR & I2C_BUSBUSY)
    {
        cumulative_timeout_cycles--;
        if (cumulative_timeout_cycles == 0)
        {
            return false;
        }
    }

    // Disable I2C during configuration
    i2c->MDR = 0;

    // Configure the I2C controller as transmitter in repeat mode
    i2cSetSlaveAdd(i2c, slaveAddress);
    i2c->MDR = I2C_RESET_OUT | I2C_TRANSMITTER | I2C_REPEATMODE;

    // Wait until the slave device acknowledges its address
    while (true)
    {
        // Set the START condition
        i2c->MDR |= I2C_START_COND | I2C_MASTER;

        // Wait for the ARDY flag
        while ((i2c->STR & I2C_ARDY) == 0)
        {
            cumulative_timeout_cycles--;
            if (cumulative_timeout_cycles == 0)
            {
                return false;
            }
        }

        // Set the STOP condition
        i2c->MDR |= I2C_STOP_COND;

        // Wait until the bus isn't busy and the master mode bit is cleared
        while (i2c->STR & I2C_BUSBUSY)
        {
            cumulative_timeout_cycles--;
            if (cumulative_timeout_cycles == 0)
            {
                return false;
            }
        }
        while (i2c->MDR & I2C_MASTER)
        {
            cumulative_timeout_cycles--;
            if (cumulative_timeout_cycles == 0)
            {
                return false;
            }
        }

        // Check if the slave address is acknowledged
        if ((i2c->STR & I2C_NACK) == 0){
            return true;
        }
        {
            // Slave address NACKed (clear the NACK bit)
            i2c->STR = I2C_NACK;
        }
    }
}

static bool sendByte(i2cBASE_t *i2c, uint8_t byte)
{
    // Quits the operation if the cumulative number of cycles spent waiting across all operations is greater than cumulative_timeout_cycles:
    uint32_t cumulative_timeout_cycles = I2C_MASTER_READ_WRITE_MAX_DELAY_CYCLES;

    // Wait for the TXRDY flag to transmit data or ARDY if we get NACKed
    while ((i2c->STR & (I2C_TX | I2C_ARDY)) == 0)
    {
        cumulative_timeout_cycles--;
        if (cumulative_timeout_cycles == 0)
        {
            return false;
        }
    }

    // If a NACK occurred then SCL is held low and STP bit cleared
    if (i2c->STR & I2C_NACK)
    {
        // Reset the I2C
        i2c->MDR = 0;
        return false;
    }

    i2c->DXR = byte;
    return true;
}

static bool receiveByte(i2cBASE_t *i2c, uint8_t *byte)
{
    // Quits the operation if the cumulative number of cycles spent waiting across all operations is greater than cumulative_timeout_cycles:
    uint32_t cumulative_timeout_cycles = I2C_MASTER_READ_WRITE_MAX_DELAY_CYCLES;

    // Wait for the RXRDY flag to transmit data or ARDY if we get NACKed
    while ((i2c->STR & (I2C_RX | I2C_ARDY)) == 0)
    {
            cumulative_timeout_cycles--;
            if (cumulative_timeout_cycles == 0)
            {
                return false;
            }
    }

    // If a NACK occurred then SCL is held low and STP bit cleared
    if (i2c->STR & I2C_NACK)
    {
        // Reset the I2C
        i2c->MDR = 0;
        return false;
    }

    // Make sure that the RXRDY flag is set
    while ((i2c->STR & I2C_RX) == 0)
    {
            cumulative_timeout_cycles--;
            if (cumulative_timeout_cycles == 0)
            {
                return false;
            }
    }

    *byte = (uint8_t)i2c->DRR;
    return true;
}

/* Read from a slave device */
bool i2cMasterReadData(i2cBASE_t *i2c, I2cSlaveAddress_t slaveAddress, uint8_t readRegAddress, uint16_t readLength, uint8 *buff)
{
    // Quits the operation if the cumulative number of cycles spent waiting across all operations is greater than cumulative_timeout_cycles:
    uint32_t cumulative_timeout_cycles = I2C_MASTER_READ_WRITE_MAX_DELAY_CYCLES;

    if (readLength == 0 || buff == NULL)
    { // note: replaced 'nullptr' with 'NULL'
        return false;
    }

    // Wait until the slave device is not busy
    if (!waitWhileDeviceIsBusy(i2c, slaveAddress))
    {
        // Wait failed (device stayed busy for longer than timeout)
        return false;
    }

    // Disable I2C during configuration
    i2c->MDR = 0;

    // Configure the I2C controller as transmitter
    i2cSetCount(i2c, 1); // 1 byte for the internal address
    i2cSetSlaveAdd(i2c, slaveAddress);
    i2c->MDR = I2C_RESET_OUT | I2C_START_COND | I2C_TRANSMITTER | I2C_MASTER;

    // Send the internal address
    if (!sendByte(i2c, readRegAddress))
    {
        return false;
    }

    // Wait for ARDY before beginning the read phase
    while ((i2c->STR & I2C_ARDY) == 0)
    {
        cumulative_timeout_cycles--;
        if (cumulative_timeout_cycles == 0)
        {
            return false;
        }
    }

    // Configure the I2C controller as receiver
    i2cSetCount(i2c, readLength);
    i2c->MDR = I2C_RESET_OUT | I2C_START_COND | I2C_STOP_COND | I2C_MASTER;

    // Receive the data
    while (readLength)
    {
        readLength--;
        if (!receiveByte(i2c, buff++))
        {
            return false;
        }
    }

    // Wait until the bus isn't busy and the master mode bit is cleared
    while (i2c->STR & I2C_BUSBUSY)
    {
        cumulative_timeout_cycles--;
        if (cumulative_timeout_cycles == 0)
        {
            return false;
        }
    }
    while (i2c->MDR & I2C_MASTER)
    {
        cumulative_timeout_cycles--;
        if (cumulative_timeout_cycles == 0)
        {
            return false;
        }
    }

    return true;
}

/* Write to a slave device */
bool i2cMasterTransmit(i2cBASE_t *i2c, I2cSlaveAddress_t slaveAddress, uint8_t writeRegAddr, uint16_t writeLength, uint8 *buff)
{
    // Quits the operation if the cumulative number of cycles spent waiting across all operations is greater than cumulative_timeout_cycles:
    uint32_t cumulative_timeout_cycles = I2C_MASTER_READ_WRITE_MAX_DELAY_CYCLES;

    if (writeLength == 0 || buff == NULL)
    { // note: replaced 'nullptr' with 'NULL'
        return false;
    }

    // Wait until the slave device is not busy
    if (!waitWhileDeviceIsBusy(i2c, slaveAddress))
    {
        // Wait failed (device stayed busy for longer than timeout)
        return false;
    }

    // Disable I2C during configuration
    i2c->MDR = 0;

    // Configure the I2C controller
    i2cSetCount(i2c, writeLength + 1); // + 1 bytes for the write reg address
    i2cSetSlaveAdd(i2c, slaveAddress);
    i2c->MDR = I2C_RESET_OUT | I2C_START_COND | I2C_STOP_COND | I2C_TRANSMITTER | I2C_MASTER;

    // Send the internal address
    if (!sendByte(i2c, writeRegAddr))
    {
        return false;
    }

    // Transmit the data
    while (writeLength)
    {
        writeLength--;
        if (!sendByte(i2c, *buff++))
        {
            return false;
        }
    }

    // Wait until the bus isn't busy and the master mode bit is cleared
    while (i2c->STR & I2C_BUSBUSY)
    {
        cumulative_timeout_cycles--;
        if (cumulative_timeout_cycles == 0)
        {
            return false;
        }
    }
    while (i2c->MDR & I2C_MASTER)
    {
        cumulative_timeout_cycles--;
        if (cumulative_timeout_cycles == 0)
        {
            return false;
        }
    }

    return true;
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
    for (i = 100000; i; --i)
    {
        volatile j = 5;
        j++;
    }
}
