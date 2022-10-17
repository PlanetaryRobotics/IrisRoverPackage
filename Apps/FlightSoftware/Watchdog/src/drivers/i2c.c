/*
 * i2c.c
 *
 *  Created on: May 13, 2021
 *      Author: mschnur
 */

#include "drivers/i2c.h"
#include "common.h"
#include <string.h>

//###########################################################
// Private definitions and globals
//###########################################################

static I2C__TransactionStatus theStatus = { 0 };

//###########################################################
// Private function declarations
//###########################################################

// Checks for an ack from the slave, stops the transmission
// if we didn't get one
static BOOL I2C__checkAck(void);

// State handler functions
static BOOL I2C__waitForStop(void);
static BOOL I2C__txStart(void);
static BOOL I2C__confirmStart(void);
static BOOL I2C__txRegAddress(void);
static BOOL I2C__txData(void);
static BOOL I2C__rxStart(void);
static BOOL I2C__rxDataAndStop(void);

//###########################################################
// Public function definitions
//###########################################################

void I2C__init(void)
{
    // Configure i2c interface
    P1SEL1 |= BIT6; // P1.6 SDA
    P1SEL1 |= BIT7; // P1.7 SCL

    UCB0CTLW0 = UCSWRST;                      // Enable SW reset
    UCB0CTLW0 |= UCMODE_3 | UCMST | UCSSEL__SMCLK | UCSYNC; // I2C master mode, SMCLK
    UCB0BRW = 160;                            // f_SCL = f_SMCLK/160 = 8 Mhz / 160 = ~50kHz
    UCB0CTLW0 &= ~UCSWRST;                    // Clear SW reset, resume operation
    UCB0IE = 0; // Disable all interrupts; we don't use them
}

I2C__Status I2C__write(uint8_t dev_addr, uint8_t reg_addr, uint8_t data)
{
    if (!((theStatus.state == I2C__TRANSACTION__UNKNOWN)
          || (theStatus.state == I2C__TRANSACTION__DONE_SUCCESS)
          || (theStatus.state == I2C__TRANSACTION__DONE_ERROR_NACK))) {
        return I2C__STATUS__ERROR__ALREADY_ACTIVE_TRANSACTION;
    }

    theStatus.devAddr = dev_addr;
    theStatus.regAddr = reg_addr;
    theStatus.type = I2C__TYPE__WRITE;
    theStatus.state = I2C__TRANSACTION__WAIT_FOR_STOP;
    theStatus.data = data;

    return I2C__STATUS__SUCCESS;
}

I2C__Status I2C__read(uint8_t dev_addr, uint8_t reg_addr)
{
    if (!((theStatus.state == I2C__TRANSACTION__UNKNOWN)
          || (theStatus.state == I2C__TRANSACTION__DONE_SUCCESS)
          || (theStatus.state == I2C__TRANSACTION__DONE_ERROR_NACK))) {
        return I2C__STATUS__ERROR__ALREADY_ACTIVE_TRANSACTION;
    }

    theStatus.devAddr = dev_addr;
    theStatus.regAddr = reg_addr;
    theStatus.type = I2C__TYPE__READ;
    theStatus.state = I2C__TRANSACTION__WAIT_FOR_STOP;
    theStatus.data = 0;

    return I2C__STATUS__SUCCESS;
}

void I2C__stop(void)
{
    if ((theStatus.state == I2C__TRANSACTION__UNKNOWN)
          || (theStatus.state == I2C__TRANSACTION__DONE_SUCCESS)
          || (theStatus.state == I2C__TRANSACTION__DONE_ERROR_NACK)) {
        return;
    }

    // Some kind of transaction is active. Setting the stop bit will generate a STOP condition in case of either a TX
    // or RX, and my understanding is that it will be automatically cleared once the STOP condition has completed.
    UCB0CTLW0 |= UCTXSTP;

    // Since the transaction should be terminated by setting the STOP condition above, clear out the transaction status
    theStatus.state = I2C__TRANSACTION__UNKNOWN;

    return;
}

I2C__Status I2C__getTransactionStatus(I2C__TransactionStatus* tStatus)
{
    if (NULL == tStatus) {
        return I2C__STATUS__ERROR__NULL;
    }

    if (theStatus.state == I2C__TRANSACTION__UNKNOWN) {
        return I2C__STATUS__ERROR__NO_TRANSACTION;
    }

    memcpy(tStatus, &theStatus, sizeof(*tStatus));
    return I2C__STATUS__SUCCESS;
}

void I2C__spinOnce(void)
{
    BOOL done = FALSE;

    while (!done) {
        switch (theStatus.state) {
            case I2C__TRANSACTION__UNKNOWN:
                // We haven't started a transaction
                done = TRUE;
                break;

            case I2C__TRANSACTION__WAIT_FOR_STOP:
                done = I2C__waitForStop();
                break;

            case I2C__TRANSACTION__TX_START:
                done = I2C__txStart();
                break;

            case I2C__TRANSACTION__CONFIRM_START:
                done = I2C__confirmStart();
                break;

            case I2C__TRANSACTION__TX_REG_ADDRESS:
                done = I2C__txRegAddress();
                break;

            case I2C__TRANSACTION__TX_DATA:
                done = I2C__txData();
                break;

            case I2C__TRANSACTION__RX_START:
                done = I2C__rxStart();
                break;

            case I2C__TRANSACTION__RX_DATA_AND_STOP:
                done = I2C__rxDataAndStop();
                break;

            case I2C__TRANSACTION__DONE_SUCCESS: // fall through
            case I2C__TRANSACTION__DONE_ERROR_NACK: // fall through
            default:
                done = TRUE;
                break;

        }
    }
}

//###########################################################
// Private function definitions
//###########################################################

static BOOL I2C__checkAck(void)
{
    BOOL result = TRUE;

    if (UCB0IFG & UCNACKIFG) {
        // Stop the current transaction
        UCB0CTLW0 |= UCTXSTP;

        // Clear the interrupt flag
        UCB0IFG &= ~UCNACKIFG;

        theStatus.state = I2C__TRANSACTION__DONE_ERROR_NACK;
        result = FALSE;
    }

    return result;
}

static BOOL I2C__waitForStop(void)
{
    BOOL continueSpinning = FALSE;

    // We just want to make sure that the STOP condition of the previous transaction
    // has cleared (i.e. taken effect)
    if ((UCB0CTLW0 & UCTXSTP) == 0U) {
        // STOP has cleared, so initiate tx of device address for the current transaction
        UCB0I2CSA = theStatus.devAddr;                 // Set the slave device address
        UCB0IFG &= ~(UCTXIFG + UCRXIFG + UCNACKIFG);   // Clear any pending interrupts
        UCB0CTLW0 |= UCTR + UCTXSTT;                   // Send the start condition

        theStatus.state = I2C__TRANSACTION__TX_START;
        continueSpinning = TRUE;
    }

    return continueSpinning;
}

static BOOL I2C__txStart(void)
{
    BOOL continueSpinning = FALSE;

    // We're looking for being ready to transmit the first data (i.e. UCTXIFG is set)
    //
    // The Start condition is not cleared until after first byte is written to UCB0TXBUF
    // and we get an acknowledgment of the slave device address, so we check for it being
    // cleared later.
    if (UCB0IFG & UCTXIFG) {
        // We need to place the next byte (register address), into the tx buffer before we
        // will get an ACK for the slave device address byte that was sent
        UCB0TXBUF = theStatus.regAddr;
        UCB0IFG &= ~UCTXIFG; // Clear the interrupt flag
        theStatus.state = I2C__TRANSACTION__CONFIRM_START;
        continueSpinning = TRUE;
    }
    return continueSpinning;
}

static BOOL I2C__confirmStart(void)
{
    BOOL continueSpinning = FALSE;
    // We're looking for the start condition to be cleared, which means we can check the ack
    // of the slave device address
    if ((UCB0CTLW0 & UCTXSTT) == 0U) {
        // Make sure we got an ACK for the device address
        if (I2C__checkAck()) {
            theStatus.state = I2C__TRANSACTION__TX_REG_ADDRESS;
            continueSpinning = TRUE;
        }
    }
    return continueSpinning;
}

static BOOL I2C__txRegAddress(void)
{
    BOOL continueSpinning = FALSE;

    // We're looking for the register address send being complete (i.e. UCTXIFG is set)
    if ((UCB0IFG & UCTXIFG)) {
        // We're ready to read or write the next byte, but first we need to make
        // sure we got an ACK for the register address
        if (I2C__checkAck()) {
            // What we do next depends on whether we're reading or writing data
            if (I2C__TYPE__READ == theStatus.type) {
                // We want to read, so switch to receiver and initiate a repeated START condition
                UCB0CTLW0 &= ~UCTR;            // Switch to receiver
                UCB0CTLW0 |= UCTXSTT;          // Send repeated start

                theStatus.state = I2C__TRANSACTION__RX_START;
            } else { /* I2C__TYPE__WRITE == theStatus.type */
                // We want to write, so put the data in the TX buffer
                UCB0TXBUF = theStatus.data;
                UCB0IFG &= ~UCTXIFG; // Clear the interrupt flag

                theStatus.state = I2C__TRANSACTION__TX_DATA;
            }

            continueSpinning = TRUE;
        }
    }

    // If we got a NACK then change state
    I2C__checkAck();

    return continueSpinning;
}

static BOOL I2C__txData(void)
{
    // We're looking for the data send being complete (i.e. UCTXIFG is set)
    if ((UCB0IFG & UCTXIFG)) {
        // We finished sending the data. Check to make sure we got an ack, and if so we're done.
        if (I2C__checkAck()) {
            // We're done, so send a STOP condition
            UCB0IFG &= ~UCTXIFG; // Clear the interrupt flag
            UCB0CTLW0 |= UCTXSTP;

            theStatus.state = I2C__TRANSACTION__DONE_SUCCESS;
            // We don't want to continue spinning after this since there is nothing left to do
        }
    }

    // If we got a NACK then change state
    I2C__checkAck();

    return FALSE;
}

static BOOL I2C__rxStart(void)
{
    BOOL continueSpinning = FALSE;

    // We're looking for the repeated START to be cleared
    if ((UCB0CTLW0 & UCTXSTT) == 0U) {
        // The start condition has been sent, so verify the slave ACK'ed its address
        if (I2C__checkAck()) {
            // We're done, so send a STOP condition with the NACK for the data that either
            // has already been received or we're still waiting to receive
            UCB0CTLW0 |= UCTXSTP;

            theStatus.state = I2C__TRANSACTION__RX_DATA_AND_STOP;
            continueSpinning = TRUE;
        }
    }

    // If we got a NACK then change state
    I2C__checkAck();

    return continueSpinning;
}

static BOOL I2C__rxDataAndStop(void)
{
    // We're looking for data to have been received (i.e. UCRXIFG is set)
    if ((UCB0IFG & UCRXIFG)) {
        // We finished receiving data, so we're done. Store the data for output
        theStatus.data = UCB0RXBUF;
        UCB0IFG &= ~UCRXIFG; // Clear the interrupt flag

        theStatus.state = I2C__TRANSACTION__DONE_SUCCESS;
        // We don't want to continue spinning after this since there is nothing left to do
    }

    // If we got a NACK then change state
    I2C__checkAck();

    return FALSE;
}


