/* --COPYRIGHT--,BSD
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * --/COPYRIGHT--*/

#include <stdint.h>
#include <msp430.h>
#include "smbus.h"
#include "smbus_nwk.h"
#include "smbus_phy.h"

static void StopTimer(void)
{
    // Stop timer
    TAxCTL &= ~MC_1;
    TAxCTL |= TACLR;
}

static void StartTimer(void)
{
    // Start timer
    TAxCTL |= TACLR + MC_1;
}

void SMBus_PHY_disable(SMBus *smbus)
{
    // Reset the USCI module, enable SW reset
    UCB0CTL1 |= UCSWRST;

    // Turn off the timer
    StopTimer();
    smbus->ctrl.bits.phyEn = 0;      // Update control flag
}

void SMBus_PHY_enable(SMBus *smbus)
{
    if(smbus->ctrl.bits.master)
    {
        SMBus_PHY_masterEnable(smbus);
    }
    else
    {
        SMBus_PHY_slaveEnable(smbus);
    }
}

void SMBus_PHY_slaveEnable(SMBus *smbus)
{
    // Make sure the PHY is disabled
    SMBus_PHY_disable(smbus);

    // Write the own slave address
    UCB0I2COA = smbus->ownSlaveAddr;

    // Clear all flags
    IFG2 = 0x00;
    UCB0STAT = 0x00;

    // Clear SW reset and start USCI I2C
    UCB0CTL1 &= ~UCSWRST;

    // Re-enable Interrupts since the Reset will clear them
    if(smbus->ctrl.bits.intEn == 1)
    {
        SMBus_slaveEnableInt(smbus);
    }

    smbus->ctrl.bits.phyEn = 1;      // Set global flag
}

void SMBus_PHY_slaveEnableInt(SMBus *smbus)
{
    // Clear all flags
    IFG2 = 0x00;
    UCB0STAT = 0x00;

    // Enable USCI Interrupts (TX, RX)
    IE2 |= UCB0RXIE | UCB0TXIE;
    // Enable USCI Interrupts (Start, Stop)
    UCB0I2CIE |= UCSTTIE | UCSTPIE;

    // Enable timout interrupt
    TAxCCTL0 = CCIE;

    // Set global flag
    smbus->ctrl.bits.intEn = 1;
}

void SMBus_PHY_slaveInit(SMBus *smbus,
                         uint16_t unused)
{
    // Set the I2C Base Address - not used in USCI implmentation
    smbus->phy.SMBus_Phy_i2cBase = 0x00;

    // Enable SW reset
    UCB0CTL1 |= UCSWRST;
    // I2C sync mode
    UCB0CTL0 |= UCMODE_3 | UCSYNC;
    // Use SMCLK, Software reset of I2C
    UCB0CTL1 |= UCSSEL_2 | UCSWRST;

    // Slave Address not initialized by default
    smbus->ownSlaveAddr = 0;

    // Set up 30ms timeout using Timer
    TAxCTL = TASSEL_1 | ID_0;           // Use ACLK for TAx
    TAxCCR0 = (uint16_t)TAxSCLTIMEOUT;  // Set timeout

    // Enable the I2C module
    SMBus_PHY_slaveEnable(smbus);
}

SMBus_State SMBus_PHY_slaveProcessInt(SMBus *smbus)
{
    SMBus_State ret_state = SMBus_State_Slave_NTR;
    uint8_t data;

    // Check if there is a RX pending and RX Interrupts are enabled
    if((IE2 & UCB0RXIE) && (IFG2 & UCB0RXIFG))
    {
        // RXIFG0 is set when a byte is received for slave address
        // Note that this ISR will be disabled and DMA will handle RX when
        // the ACK/NACK workaround is enabled

        // Get the data from the buffer
        data = UCB0RXBUF;
        // Pass data to NWK layer
        ret_state = SMBus_NWK_slaveProcessRx(smbus, data);
    }
    else if(UCB0STAT & UCSTTIFG)
    {
        StartTimer();
        // Get received address
        data = smbus->ownSlaveAddr << 1;

        if(UCB0CTL1 & UCTR)
        {
            // If Master is requesting data, add Read bit to Address+RW
            data |= I2C_READ;
        }
        else
        {
            // Clear TX flag if set in case it's still set from previous transfer
            IFG2 &= ~UCB0TXIFG;
        }
        // Pass information to NWK
        ret_state = SMBus_NWK_slaveProcessStart(smbus, data);

        // Clear start flag
        UCB0STAT &= ~UCSTTIFG;
    }
    else if(IFG2 & UCB0TXIFG)
    {
        // TXIFG0 is set when host is requesting a byte from our slave address
        // Inform the NWK layer and get the data which will be transmitted
        ret_state = SMBus_NWK_slaveProcessTx(smbus, &data);
        // Send the data
        UCB0TXBUF = data;
    }
    else if(UCB0STAT & UCSTPIFG)
    {
        StopTimer();

        // Check if a stop is pending
        // Inform the NWK later, so it can process the whole packet
        ret_state = SMBus_NWK_slaveProcessStop(smbus);
        // Clear flag
        UCB0STAT &= ~UCSTPIFG;

        if((IFG2 & UCB0TXIFG) && !(UCB0STAT & UCSTTIFG))
        {
            // Clear TX flag if set and no start was received
            IFG2 &= ~UCB0TXIFG;
        }
    }

    return(ret_state);
}

SMBus_State SMBus_PHY_slaveProcessTimeoutInt(SMBus *smbus)
{
    return(SMBus_NWK_slaveProcessTimeout(smbus));
}

void SMBus_PHY_masterEnable(SMBus *smbus)
{
    // Make sure the PHY is disabled
    SMBus_PHY_disable(smbus);

    // Write the own slave address
    UCB0I2COA = smbus->ownSlaveAddr;

    // Clear all flags
    IFG2 &= ~(UCB0TXIFG | UCB0RXIFG);
    UCB0STAT = 0x00;

    // Clear SW reset and start USCI I2C
    UCB0CTL1 &= ~UCSWRST;

    // Re-enable Interrupts since the Reset will clear them
    if(smbus->ctrl.bits.intEn == 1)
    {
        SMBus_masterEnableInt(smbus);
    }

    smbus->ctrl.bits.phyEn = 1;      // Set global flag
}

void SMBus_PHY_masterEnableInt(SMBus *smbus)
{
    // Clear all flags
    IFG2 &= ~(UCB0TXIFG | UCB0RXIFG);
    UCB0STAT = 0x00;

    // Enable USCI Interrupts (TX, RX)
    IE2 |= UCB0RXIE + UCB0TXIE;
    // Enable USCI Interrupts (Start, NACK, Arbitration Lost)
    UCB0I2CIE |= UCSTTIE + UCNACKIE + UCALIE;

    // Enable timout interrupt
    TAxCCTL0 = CCIE;

    // Set global flag
    smbus->ctrl.bits.intEn = 1;
}

void SMBus_PHY_masterInit(SMBus *smbus,
                          uint16_t unused,
                          uint32_t busClk)
{
    uint16_t Prescaler;

    // Set the I2C Base Address - not used in USCI implmentation
    smbus->phy.SMBus_Phy_i2cBase = 0x00;

    // Enable SW reset
    UCB0CTL1 |= UCSWRST;
    // I2C multi-master mode, sync mode, Multi-Master mode
    UCB0CTL0 |= UCMST + UCMM + UCMODE_3 + UCSYNC;
    // Use SMCLK, Software reset of I2C
    UCB0CTL1 |= UCSSEL_2 + UCSWRST;

    // Slave Address not initialized by default
    UCB0I2CSA = 0x00;
    smbus->ownSlaveAddr = 0;

    // PreStop is cleared
    smbus->phy.SMBus_Phy_stop = SMBus_Stop_No;

    //Compute the prescaler. Per SMBus spec, the frequency is always 100Khz
    Prescaler = (uint16_t )(busClk / 100000);
    UCB0BR0 = (Prescaler & 0xFF);
    UCB0BR1 = ((Prescaler >> 8) & 0xFF);

    // Set up 30ms timeout using Timer
    TAxCTL = TASSEL_1 | ID_0;           // Use ACLK for TAx
    TAxCCR0 = (uint16_t)TAxSCLTIMEOUT;  // Set timeout

    // Enable the I2C module
    SMBus_PHY_masterEnable(smbus);
}

void SMBus_PHY_masterSendStop(SMBus *smbus)
{
    // Generate Stop condition if it hasn't been sent
    if(smbus->phy.SMBus_Phy_stop != SMBus_Stop_Sent)
    {
        UCB0CTL1 |= UCTXSTP;
        smbus->phy.SMBus_Phy_stop = SMBus_Stop_Sent;
    }

    // Wait for the Stop to be sent
    while(UCB0CTL1 & UCTXSTP)
    {
        ;
    }
    StopTimer();

    // Stop will get auto-cleared, assume it has ocurred
    SMBus_NWK_masterProcessStop(smbus);
}

void SMBus_PHY_masterSendPreStop(SMBus *smbus)
{
    smbus->phy.SMBus_Phy_stop = SMBus_Stop_PreRead;
}

void SMBus_PHY_masterStartTx(SMBus *smbus,
                             uint8_t targetaddr,
                             SMBus_Stop stopFlag)
{
    // Set target slave addr
    UCB0I2CSA = targetaddr;

    smbus->phy.SMBus_Phy_stop = SMBus_Stop_No;
    if(stopFlag == SMBus_Stop_Immediate)
    {
        // Master mode, transmit mode, transmit start/stop
        UCB0CTL0 |= UCMST;
        UCB0CTL1 |= UCTR + UCTXSTT + UCTXSTP;

        smbus->phy.SMBus_Phy_stop = SMBus_Stop_Sent;

        // Send and process Stop
        SMBus_PHY_masterSendStop(smbus);
    }
    else
    {
        // Master mode, transmit mode, transmit start
        UCB0CTL0 |= UCMST;
        UCB0CTL1 |= UCTR + UCTXSTT;

        StartTimer();
    }
}

void SMBus_PHY_masterStartRx(SMBus *smbus,
                             uint8_t targetaddr,
                             SMBus_Stop stopFlag)
{
    // Set target slave addr
    UCB0I2CSA = targetaddr;

    // Set as receiver
    UCB0CTL1 &= ~UCTR;

    smbus->phy.SMBus_Phy_stop = SMBus_Stop_No;
    if(stopFlag == SMBus_Stop_Immediate)
    {
        // Master mode, transmit start, transmit stop
        UCB0CTL0 |= UCMST;
        UCB0CTL1 |= UCTXSTT | UCTXSTP;
        smbus->phy.SMBus_Phy_stop = SMBus_Stop_Sent;

        // Stop will get auto-cleared, assume it has ocurred
        SMBus_NWK_masterProcessStop(smbus);
    }
    else
    {
        // Master mode, transmit start
        UCB0CTL0 |= UCMST;
        UCB0CTL1 |= UCTXSTT;

        StartTimer();
    }

    if(stopFlag == SMBus_Stop_Addr)
    {
        // Wait for the Start to be sent
        while(UCB0CTL1 & UCTXSTT)
        {
            ;
        }
        SMBus_PHY_masterSendStop(smbus);
    }

    if(stopFlag == SMBus_Stop_PreRead)
    {
        smbus->phy.SMBus_Phy_stop = SMBus_Stop_PreRead;
    }
}

SMBus_State SMBus_PHY_masterProcessInt(SMBus *smbus)
{
    uint8_t data;
    SMBus_State ret_state;

    // Check for NACK
    if(UCB0STAT & UCNACKIFG)
    {
        // Pass data to NWK layer
        ret_state = SMBus_NWK_masterProcessNACK(smbus);
        // Clear flag
        UCB0STAT &= ~UCNACKIFG;
        // Per Erata USCI25, ensure UCB0TXIFG is reset
        IFG2 &= ~UCB0TXIFG;
    }
    else if(UCB0STAT & UCALIFG)
    {
        // Arbitration lost
        // Clear flag
        UCB0STAT &= ~UCALIFG;
    }
    else if(UCB0STAT & UCSTTIFG)
    {
        // Start is detected when we get a packet as Slave
        // Clear start flag
        UCB0STAT &= ~UCSTTIFG;
    }
    else if(IFG2 & UCB0RXIFG) // Check if there is a RX pending
    {
        uint16_t count;
        // Workaround for USCI30, wait at least 3 bit clocks with SCL Low
        // considering the time it takes to execute this routine,
        // we need to wait ~2.14 counts * MCLK_MHZ (rounded to 3*MCLK_MHZ)
        if(smbus->phy.SMBus_Phy_stop != SMBus_Stop_Sent)
        {
            do
            {
                count = 0;
                while((UCB0STAT & UCSCLLOW) &&
                      (count++ < (MCLK_MHZ * 3)))
                {
                    ;
                }
            }
            while(count < (MCLK_MHZ * 3));
        }
        // Send stop before receiving last byte
        if(smbus->phy.SMBus_Phy_stop == SMBus_Stop_PreRead)
        {
            UCB0CTL1 |= UCTXSTP;
            data = UCB0RXBUF;
            smbus->phy.SMBus_Phy_stop = SMBus_Stop_Sent;

            StopTimer();
        }
        else
        {
            data = UCB0RXBUF;
        }
        // Pass data to NWK layer
        ret_state = SMBus_NWK_masterProcessRx(smbus, data);
    }
    else if(IFG2 & UCB0TXIFG)
    {
        // TXIFG is set when the TX buffer is empty and we need to send data to
        // slave
        ret_state = SMBus_NWK_masterProcessTx(smbus, &data);
        // Send the data
        UCB0TXBUF = data;
    }

    smbus->state = ret_state;

    return(ret_state);
}

SMBus_State SMBus_PHY_masterProcessTimeoutInt(SMBus *smbus)
{
    return(SMBus_NWK_masterProcessTimeout(smbus));
}
