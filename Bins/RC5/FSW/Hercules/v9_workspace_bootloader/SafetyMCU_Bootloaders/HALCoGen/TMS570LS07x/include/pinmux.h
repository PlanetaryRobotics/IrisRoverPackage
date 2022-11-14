/** @file pinmux.h
*   @brief PINMUX Driver Implementation File
*   @date 11-Dec-2018
*   @version 04.07.01
*
*/

/* 
* Copyright (C) 2009-2018 Texas Instruments Incorporated - www.ti.com  
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


#ifndef __PINMUX_H__
#define __PINMUX_H__

#include "reg_pinmux.h"

#ifdef __cplusplus
extern "C" {
#endif
#define PINMUX_PIN_1_SHIFT                  8U
#define PINMUX_PIN_2_SHIFT                  0U
#define PINMUX_PIN_5_SHIFT                  0U
#define PINMUX_PIN_10_SHIFT                 24U
#define PINMUX_PIN_11_SHIFT                 8U
#define PINMUX_PIN_12_SHIFT                 16U
#define PINMUX_PIN_18_SHIFT                 0U
#define PINMUX_PIN_19_SHIFT                 0U
#define PINMUX_PIN_22_SHIFT                 8U
#define PINMUX_PIN_25_SHIFT                 0U
#define PINMUX_PIN_26_SHIFT                 16U
#define PINMUX_PIN_27_SHIFT                 8U
#define PINMUX_PIN_34_SHIFT                 8U
#define PINMUX_PIN_35_SHIFT                 16U
#define PINMUX_PIN_36_SHIFT                 24U
#define PINMUX_PIN_37_SHIFT                 8U
#define PINMUX_PIN_38_SHIFT                 16U
#define PINMUX_PIN_64_SHIFT                 24U
#define PINMUX_PIN_68_SHIFT                 16U
#define PINMUX_PIN_73_SHIFT                 24U
#define PINMUX_PIN_74_SHIFT                 0U
#define PINMUX_PIN_83_SHIFT                 0U
#define PINMUX_PIN_93_SHIFT                 16U
#define PINMUX_PIN_90_SHIFT                 8U
#define PINMUX_PIN_97_SHIFT                 0U
#define PINMUX_PIN_98_SHIFT                 8U


#define PINMUX_GATE_EMIF_CLK_SHIFT          8U
#define PINMUX_GIOB_DISABLE_HET2_SHIFT      16U
#define PINMUX_ALT_ADC_TRIGGER_SHIFT        0U
#define PINMUX_ETHERNET_SHIFT               24U
#define PINMUX_ETPWM1_SHIFT                 0U
#define PINMUX_ETPWM2_SHIFT                 8U
#define PINMUX_ETPWM3_SHIFT                 16U
#define PINMUX_ETPWM4_SHIFT                 24U
#define PINMUX_ETPWM5_SHIFT                 0U
#define PINMUX_ETPWM6_SHIFT                 8U
#define PINMUX_ETPWM7_SHIFT                 16U
#define PINMUX_ETPWM_TIME_BASE_SYNC_SHIFT   24U
#define PINMUX_ETPWM_TBCLK_SYNC_SHIFT       0U
#define PINMUX_TZ1_SHIFT                    16U
#define PINMUX_TZ2_SHIFT                    24U
#define PINMUX_TZ3_SHIFT                    0U
#define PINMUX_EPWM1SYNCI_SHIFT             8U

#define PINMUX_PIN_1_MASK                   (~(uint32)((uint32)0xFFU << PINMUX_PIN_1_SHIFT))
#define PINMUX_PIN_2_MASK                   (~(uint32)((uint32)0xFFU << PINMUX_PIN_2_SHIFT))
#define PINMUX_PIN_5_MASK                   (~(uint32)((uint32)0xFFU << PINMUX_PIN_5_SHIFT))
#define PINMUX_PIN_10_MASK                   (~(uint32)((uint32)0xFFU << PINMUX_PIN_10_SHIFT))
#define PINMUX_PIN_11_MASK                   (~(uint32)((uint32)0xFFU << PINMUX_PIN_11_SHIFT))
#define PINMUX_PIN_12_MASK                   (~(uint32)((uint32)0xFFU << PINMUX_PIN_12_SHIFT))
#define PINMUX_PIN_18_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_18_SHIFT))
#define PINMUX_PIN_19_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_19_SHIFT))
#define PINMUX_PIN_22_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_22_SHIFT))
#define PINMUX_PIN_25_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_25_SHIFT))
#define PINMUX_PIN_26_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_26_SHIFT))
#define PINMUX_PIN_27_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_27_SHIFT))
#define PINMUX_PIN_34_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_34_SHIFT))
#define PINMUX_PIN_35_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_35_SHIFT))
#define PINMUX_PIN_36_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_36_SHIFT))
#define PINMUX_PIN_37_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_37_SHIFT))
#define PINMUX_PIN_38_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_38_SHIFT))
#define PINMUX_PIN_64_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_64_SHIFT))
#define PINMUX_PIN_68_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_68_SHIFT))
#define PINMUX_PIN_73_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_73_SHIFT))
#define PINMUX_PIN_74_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_74_SHIFT))
#define PINMUX_PIN_83_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_83_SHIFT))
#define PINMUX_PIN_90_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_90_SHIFT))
#define PINMUX_PIN_93_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_93_SHIFT))
#define PINMUX_PIN_97_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_97_SHIFT))
#define PINMUX_PIN_98_MASK                  (~(uint32)((uint32)0xFFU << PINMUX_PIN_98_SHIFT))

#define PINMUX_GATE_EMIF_CLK_MASK           (~(uint32)((uint32)0xFFU << PINMUX_GATE_EMIF_CLK_SHIFT))
#define PINMUX_GIOB_DISABLE_HET2_MASK       (~(uint32)((uint32)0xFFU << PINMUX_GIOB_DISABLE_HET2_SHIFT))
#define PINMUX_ALT_ADC_TRIGGER_MASK         (~(uint32)((uint32)0xFFU << PINMUX_ALT_ADC_TRIGGER_SHIFT))
#define PINMUX_ETHERNET_MASK                (~(uint32)((uint32)0xFFU << PINMUX_ETHERNET_SHIFT))

#define PINMUX_ETPWM1_MASK                   (~(uint32)((uint32)0xFFU << PINMUX_ETPWM1_SHIFT))
#define PINMUX_ETPWM2_MASK                   (~(uint32)((uint32)0xFFU << PINMUX_ETPWM2_SHIFT))
#define PINMUX_ETPWM3_MASK                   (~(uint32)((uint32)0xFFU << PINMUX_ETPWM3_SHIFT))
#define PINMUX_ETPWM4_MASK                   (~(uint32)((uint32)0xFFU << PINMUX_ETPWM4_SHIFT))
#define PINMUX_ETPWM5_MASK                   (~(uint32)((uint32)0xFFU << PINMUX_ETPWM5_SHIFT))
#define PINMUX_ETPWM6_MASK                   (~(uint32)((uint32)0xFFU << PINMUX_ETPWM6_SHIFT))
#define PINMUX_ETPWM7_MASK                   (~(uint32)((uint32)0xFFU << PINMUX_ETPWM7_SHIFT))
#define PINMUX_ETPWM_TIME_BASE_SYNC_MASK     (~(uint32)((uint32)0xFFU << PINMUX_ETPWM_TIME_BASE_SYNC_SHIFT))
#define PINMUX_ETPWM_TBCLK_SYNC_MASK         (~(uint32)((uint32)0xFFU << PINMUX_ETPWM_TBCLK_SYNC_SHIFT))
#define PINMUX_TZ1_MASK                      (~(uint32)((uint32)0xFFU << PINMUX_TZ1_SHIFT))
#define PINMUX_TZ2_MASK                      (~(uint32)((uint32)0xFFU << PINMUX_TZ2_SHIFT))
#define PINMUX_TZ3_MASK                      (~(uint32)((uint32)0xFFU << PINMUX_TZ3_SHIFT))
#define PINMUX_EPWM1SYNCI_MASK               (~(uint32)((uint32)0xFFU << PINMUX_EPWM1SYNCI_SHIFT))




#define PINMUX_PIN_5_GIOA_2                  ((uint32)((uint32)0x1U <<  PINMUX_PIN_5_SHIFT))
#define PINMUX_PIN_5_HET2_0                  ((uint32)((uint32)0x8U <<  PINMUX_PIN_5_SHIFT))
#define PINMUX_PIN_5_EQEP2I                  ((uint32)((uint32)0x10U <<  PINMUX_PIN_5_SHIFT))

#define PINMUX_PIN_10_GIOA_5                 ((uint32)((uint32)0x1U <<  PINMUX_PIN_10_SHIFT))
#define PINMUX_PIN_10_EXTCLKIN               ((uint32)((uint32)0x2U <<  PINMUX_PIN_10_SHIFT))
#define PINMUX_PIN_10_ETPWM1A                ((uint32)((uint32)0x4U <<  PINMUX_PIN_10_SHIFT))

#define PINMUX_PIN_12_GIOA_6                 ((uint32)((uint32)0x1U <<  PINMUX_PIN_12_SHIFT))
#define PINMUX_PIN_12_HET2_4                 ((uint32)((uint32)0x2U <<  PINMUX_PIN_12_SHIFT))
#define PINMUX_PIN_12_ETPWM1B                ((uint32)((uint32)0x4U <<  PINMUX_PIN_12_SHIFT))

#define PINMUX_PIN_18_GIOA_7                 ((uint32)((uint32)0x1U <<  PINMUX_PIN_18_SHIFT))
#define PINMUX_PIN_18_HET2_6                 ((uint32)((uint32)0x2U <<  PINMUX_PIN_18_SHIFT))
#define PINMUX_PIN_18_ETPWM2A                ((uint32)((uint32)0x4U <<  PINMUX_PIN_18_SHIFT))

#define PINMUX_PIN_19_HET1_0                 ((uint32)((uint32)0x1U <<  PINMUX_PIN_19_SHIFT))
#define PINMUX_PIN_19_SPI4CLK                ((uint32)((uint32)0x2U <<  PINMUX_PIN_19_SHIFT))
#define PINMUX_PIN_19_ETPWM2B                ((uint32)((uint32)0x4U <<  PINMUX_PIN_19_SHIFT))

#define PINMUX_PIN_22_HET1_02                ((uint32)((uint32)0x1U <<  PINMUX_PIN_22_SHIFT))
#define PINMUX_PIN_22_SPI4SIMO               ((uint32)((uint32)0x2U <<  PINMUX_PIN_22_SHIFT))
#define PINMUX_PIN_22_ETPWM3A                ((uint32)((uint32)0x4U <<  PINMUX_PIN_22_SHIFT))


#define PINMUX_PIN_25_HET1_04                ((uint32)((uint32)0x1U <<  PINMUX_PIN_25_SHIFT))
#define PINMUX_PIN_25_ETPWM4B                ((uint32)((uint32)0x2U <<  PINMUX_PIN_25_SHIFT))

#define PINMUX_PIN_26_HET1_06                ((uint32)((uint32)0x1U <<  PINMUX_PIN_26_SHIFT))
#define PINMUX_PIN_26_SCIRX                  ((uint32)((uint32)0x2U <<  PINMUX_PIN_26_SHIFT))
#define PINMUX_PIN_26_ETPWM5A                ((uint32)((uint32)0x4U <<  PINMUX_PIN_26_SHIFT))

#define PINMUX_PIN_27_MIBSPI1NCS_2           ((uint32)((uint32)0x1U <<  PINMUX_PIN_27_SHIFT))
#define PINMUX_PIN_27_HET1_19                ((uint32)((uint32)0x2U <<  PINMUX_PIN_27_SHIFT))

#define PINMUX_PIN_34_MIBSPI3SOMI            ((uint32)((uint32)0x1U <<  PINMUX_PIN_34_SHIFT))
#define PINMUX_PIN_34_AWM_EXT_ENA            ((uint32)((uint32)0x2U <<  PINMUX_PIN_34_SHIFT))
#define PINMUX_PIN_34_ECAP2                  ((uint32)((uint32)0x4U <<  PINMUX_PIN_34_SHIFT))

#define PINMUX_PIN_35_MIBSPI3SIMO            ((uint32)((uint32)0x1U <<  PINMUX_PIN_35_SHIFT))
#define PINMUX_PIN_35_AWM_EXT_SEL_0          ((uint32)((uint32)0x2U <<  PINMUX_PIN_35_SHIFT))
#define PINMUX_PIN_35_ECAP3                  ((uint32)((uint32)0x4U <<  PINMUX_PIN_35_SHIFT))

#define PINMUX_PIN_36_MIBSPI3CLK             ((uint32)((uint32)0x1U <<  PINMUX_PIN_36_SHIFT))
#define PINMUX_PIN_36_AWM_EXT_SEL_1          ((uint32)((uint32)0x2U <<  PINMUX_PIN_36_SHIFT))
#define PINMUX_PIN_36_EQEP1A                 ((uint32)((uint32)0x4U <<  PINMUX_PIN_36_SHIFT))

#define PINMUX_PIN_37_MIBSPI3NENA            ((uint32)((uint32)0x1U <<  PINMUX_PIN_37_SHIFT))
#define PINMUX_PIN_37_MIBSPI3NCS_5           ((uint32)((uint32)0x2U <<  PINMUX_PIN_37_SHIFT))
#define PINMUX_PIN_37_HET1_31                ((uint32)((uint32)0x4U <<  PINMUX_PIN_37_SHIFT))
#define PINMUX_PIN_37_EQEP1B                 ((uint32)((uint32)0x8U <<  PINMUX_PIN_37_SHIFT))

#define PINMUX_PIN_38_MIBSPI3NCS_0           ((uint32)((uint32)0x1U <<  PINMUX_PIN_38_SHIFT))
#define PINMUX_PIN_38_AD2EVT                 ((uint32)((uint32)0x2U <<  PINMUX_PIN_38_SHIFT))
#define PINMUX_PIN_38_GIOB_2                 ((uint32)((uint32)0x4U <<  PINMUX_PIN_38_SHIFT))
#define PINMUX_PIN_38_EQEP1I                 ((uint32)((uint32)0x8U <<  PINMUX_PIN_38_SHIFT))

#define PINMUX_PIN_64_HET1_24                ((uint32)((uint32)0x1U <<  PINMUX_PIN_64_SHIFT))
#define PINMUX_PIN_64_MIBSPI1NCS_5           ((uint32)((uint32)0x2U <<  PINMUX_PIN_64_SHIFT))


#define PINMUX_PIN_68_MIBSPI1NENA            ((uint32)((uint32)0x1U <<  PINMUX_PIN_68_SHIFT))
#define PINMUX_PIN_68_HET1_23                ((uint32)((uint32)0x2U <<  PINMUX_PIN_68_SHIFT))
#define PINMUX_PIN_68_ECAP4                  ((uint32)((uint32)0x10U <<  PINMUX_PIN_68_SHIFT))


#define PINMUX_PIN_73_MIBSPI1NCS_0          ((uint32)((uint32)0x1U <<  PINMUX_PIN_73_SHIFT))
#define PINMUX_PIN_73_MIBSPI1SOMI_1         ((uint32)((uint32)0x2U <<  PINMUX_PIN_73_SHIFT))
#define PINMUX_PIN_73_ECAP6                 ((uint32)((uint32)0x10U <<  PINMUX_PIN_73_SHIFT))

#define PINMUX_PIN_74_HET1_08               ((uint32)((uint32)0x1U <<  PINMUX_PIN_74_SHIFT))
#define PINMUX_PIN_74_MIBSPI1SIMO_1         ((uint32)((uint32)0x2U <<  PINMUX_PIN_74_SHIFT))

#define PINMUX_PIN_83_HET1_10               ((uint32)((uint32)0x1U <<  PINMUX_PIN_83_SHIFT))
#define PINMUX_PIN_83_nTZ3                  ((uint32)((uint32)0x10U <<  PINMUX_PIN_83_SHIFT))

#define PINMUX_PIN_93_MIBSPI1NCS_1          ((uint32)((uint32)0x1U <<  PINMUX_PIN_93_SHIFT))
#define PINMUX_PIN_93_HET1_17               ((uint32)((uint32)0x2U <<  PINMUX_PIN_93_SHIFT))
#define PINMUX_PIN_93_EQEP1S                ((uint32)((uint32)0x10U <<  PINMUX_PIN_93_SHIFT))

#define PINMUX_PIN_97_HET1_16               ((uint32)((uint32)0x1U <<  PINMUX_PIN_97_SHIFT))
#define PINMUX_PIN_97_ETPWM1SYNCI           ((uint32)((uint32)0x2U <<  PINMUX_PIN_97_SHIFT))
#define PINMUX_PIN_97_ETPWM1SYNCO           ((uint32)((uint32)0x4U <<  PINMUX_PIN_97_SHIFT))

#define PINMUX_PIN_98_HET1_18               ((uint32)((uint32)0x1U <<  PINMUX_PIN_98_SHIFT))
#define PINMUX_PIN_98_ETPWM6A               ((uint32)((uint32)0x2U <<  PINMUX_PIN_98_SHIFT))

#define PINMUX_PIN_1_GIOA_0                  ((uint32)((uint32)0x1U <<  PINMUX_PIN_1_SHIFT))

#define PINMUX_PIN_2_GIOA_1                  ((uint32)((uint32)0x1U <<  PINMUX_PIN_2_SHIFT))

#define PINMUX_PIN_11_HET1_22                ((uint32)((uint32)0x1U <<  PINMUX_PIN_11_SHIFT))

#define PINMUX_PIN_90_HET1_14               ((uint32)((uint32)0x1U <<  PINMUX_PIN_90_SHIFT))

#define PINMUX_GATE_EMIF_CLK_ON              ((uint32)((uint32)0x0 <<  PINMUX_GATE_EMIF_CLK_SHIFT))
#define PINMUX_GIOB_DISABLE_HET2_ON          ((uint32)((uint32)0x1U <<  PINMUX_GIOB_DISABLE_HET2_SHIFT))
#define PINMUX_GATE_EMIF_CLK_OFF             ((uint32)((uint32)0x1U <<  PINMUX_GATE_EMIF_CLK_SHIFT))
#define PINMUX_GIOB_DISABLE_HET2_OFF         ((uint32)((uint32)0x0 <<  PINMUX_GIOB_DISABLE_HET2_SHIFT))
#define PINMUX_ALT_ADC_TRIGGER_1             ((uint32)((uint32)0x1U <<  PINMUX_ALT_ADC_TRIGGER_SHIFT))
#define PINMUX_ALT_ADC_TRIGGER_2             ((uint32)((uint32)0x2U <<  PINMUX_ALT_ADC_TRIGGER_SHIFT))
#define PINMUX_ETHERNET_MII                  ((uint32)((uint32)0x0 <<  PINMUX_ETHERNET_SHIFT))
#define PINMUX_ETHERNET_RMII                 ((uint32)((uint32)0x1U <<  PINMUX_ETHERNET_SHIFT))

#define PINMUX_ETPWM1_EQEPERR12               ((uint32)((uint32)0x1U <<  PINMUX_ETPWM1_SHIFT))
#define PINMUX_ETPWM1_EQEPERR1                ((uint32)((uint32)0x2U <<  PINMUX_ETPWM1_SHIFT))
#define PINMUX_ETPWM1_EQEPERR2                ((uint32)((uint32)0x4U <<  PINMUX_ETPWM1_SHIFT))
#define PINMUX_ETPWM2_EQEPERR12               ((uint32)((uint32)0x1U <<  PINMUX_ETPWM2_SHIFT))
#define PINMUX_ETPWM2_EQEPERR1                ((uint32)((uint32)0x2U <<  PINMUX_ETPWM2_SHIFT))
#define PINMUX_ETPWM2_EQEPERR2                ((uint32)((uint32)0x4U <<  PINMUX_ETPWM2_SHIFT))
#define PINMUX_ETPWM3_EQEPERR12               ((uint32)((uint32)0x1U <<  PINMUX_ETPWM3_SHIFT))
#define PINMUX_ETPWM3_EQEPERR1                ((uint32)((uint32)0x2U <<  PINMUX_ETPWM3_SHIFT))
#define PINMUX_ETPWM3_EQEPERR2                ((uint32)((uint32)0x4U <<  PINMUX_ETPWM3_SHIFT))
#define PINMUX_ETPWM4_EQEPERR12               ((uint32)((uint32)0x1U <<  PINMUX_ETPWM4_SHIFT))
#define PINMUX_ETPWM4_EQEPERR1                ((uint32)((uint32)0x2U <<  PINMUX_ETPWM4_SHIFT))
#define PINMUX_ETPWM4_EQEPERR2                ((uint32)((uint32)0x4U <<  PINMUX_ETPWM4_SHIFT))
#define PINMUX_ETPWM5_EQEPERR12               ((uint32)((uint32)0x1U <<  PINMUX_ETPWM5_SHIFT))
#define PINMUX_ETPWM5_EQEPERR1                ((uint32)((uint32)0x2U <<  PINMUX_ETPWM5_SHIFT))
#define PINMUX_ETPWM5_EQEPERR2                ((uint32)((uint32)0x4U <<  PINMUX_ETPWM5_SHIFT))
#define PINMUX_ETPWM6_EQEPERR12               ((uint32)((uint32)0x1U <<  PINMUX_ETPWM6_SHIFT))
#define PINMUX_ETPWM6_EQEPERR1                ((uint32)((uint32)0x2U <<  PINMUX_ETPWM6_SHIFT))
#define PINMUX_ETPWM6_EQEPERR2                ((uint32)((uint32)0x4U <<  PINMUX_ETPWM6_SHIFT))
#define PINMUX_ETPWM7_EQEPERR12               ((uint32)((uint32)0x1U <<  PINMUX_ETPWM7_SHIFT))
#define PINMUX_ETPWM7_EQEPERR1                ((uint32)((uint32)0x2U <<  PINMUX_ETPWM7_SHIFT))
#define PINMUX_ETPWM7_EQEPERR2                ((uint32)((uint32)0x4U <<  PINMUX_ETPWM7_SHIFT))
#define PINMUX_ETPWM_TIME_BASE_SYNC_ON        ((uint32)((uint32)0x2U <<  PINMUX_ETPWM_TIME_BASE_SYNC_SHIFT))
#define PINMUX_ETPWM_TBCLK_SYNC_ON            ((uint32)((uint32)0x2U <<  PINMUX_ETPWM_TBCLK_SYNC_SHIFT))
#define PINMUX_ETPWM_TIME_BASE_SYNC_OFF       ((uint32)((uint32)0x0 <<  PINMUX_ETPWM_TIME_BASE_SYNC_SHIFT))
#define PINMUX_ETPWM_TBCLK_SYNC_OFF           ((uint32)((uint32)0x0 <<  PINMUX_ETPWM_TBCLK_SYNC_SHIFT))
#define PINMUX_TZ1_ASYNC                      ((uint32)((uint32)0x1U <<  PINMUX_TZ1_SHIFT))
#define PINMUX_TZ1_SYNC                       ((uint32)((uint32)0x2U <<  PINMUX_TZ1_SHIFT))
#define PINMUX_TZ1_FILTERED                   ((uint32)((uint32)0x4U <<  PINMUX_TZ1_SHIFT))
#define PINMUX_TZ2_ASYNC                      ((uint32)((uint32)0x1U <<  PINMUX_TZ2_SHIFT))
#define PINMUX_TZ2_SYNC                       ((uint32)((uint32)0x2U <<  PINMUX_TZ2_SHIFT))
#define PINMUX_TZ2_FILTERED                   ((uint32)((uint32)0x4U <<  PINMUX_TZ2_SHIFT))
#define PINMUX_TZ3_ASYNC                      ((uint32)((uint32)0x1U <<  PINMUX_TZ3_SHIFT))
#define PINMUX_TZ3_SYNC                       ((uint32)((uint32)0x2U <<  PINMUX_TZ3_SHIFT))
#define PINMUX_TZ3_FILTERED                   ((uint32)((uint32)0x4U <<  PINMUX_TZ3_SHIFT))
#define PINMUX_EPWM1SYNCI_ASYNC               ((uint32)((uint32)0x1U <<  PINMUX_EPWM1SYNCI_SHIFT))
#define PINMUX_EPWM1SYNCI_SYNC                ((uint32)((uint32)0x2U <<  PINMUX_EPWM1SYNCI_SHIFT))
#define PINMUX_EPWM1SYNCI_FILTERED            ((uint32)((uint32)0x4U <<  PINMUX_EPWM1SYNCI_SHIFT))

typedef struct pinmux_config_reg
{
    uint32 CONFIG_PINMMR0;
    uint32 CONFIG_PINMMR1;
    uint32 CONFIG_PINMMR2;
    uint32 CONFIG_PINMMR3;
    uint32 CONFIG_PINMMR4;
    uint32 CONFIG_PINMMR5;
    uint32 CONFIG_PINMMR6;
    uint32 CONFIG_PINMMR7;
    uint32 CONFIG_PINMMR8;
    uint32 CONFIG_PINMMR9;
    uint32 CONFIG_PINMMR10;
    uint32 CONFIG_PINMMR11;
    uint32 CONFIG_PINMMR12;
    uint32 CONFIG_PINMMR13;
    uint32 CONFIG_PINMMR14;
    uint32 CONFIG_PINMMR15;
    uint32 CONFIG_PINMMR16;
    uint32 CONFIG_PINMMR17;
    uint32 CONFIG_PINMMR18;
    uint32 CONFIG_PINMMR19;
    uint32 CONFIG_PINMMR20;
    uint32 CONFIG_PINMMR21;
    uint32 CONFIG_PINMMR22;
    uint32 CONFIG_PINMMR23;
    uint32 CONFIG_PINMMR24;
    uint32 CONFIG_PINMMR25;
    uint32 CONFIG_PINMMR26;
    uint32 CONFIG_PINMMR27;
    uint32 CONFIG_PINMMR28;
    uint32 CONFIG_PINMMR29;
    uint32 CONFIG_PINMMR30;
    uint32 CONFIG_PINMMR31;
    uint32 CONFIG_PINMMR32;
    uint32 CONFIG_PINMMR33;
    uint32 CONFIG_PINMMR34;
    uint32 CONFIG_PINMMR35;
    uint32 CONFIG_PINMMR36;
    uint32 CONFIG_PINMMR37;
    uint32 CONFIG_PINMMR38;
    uint32 CONFIG_PINMMR39;
    uint32 CONFIG_PINMMR40;
    uint32 CONFIG_PINMMR41;
    uint32 CONFIG_PINMMR42;
    uint32 CONFIG_PINMMR43;
    uint32 CONFIG_PINMMR44;
    uint32 CONFIG_PINMMR45;
    uint32 CONFIG_PINMMR46;
    uint32 CONFIG_PINMMR47;
}pinmux_config_reg_t;

/**
 *  @defgroup IOMM IOMM
 *  @brief I/O Multiplexing and Control Module.
 *
 *  The IOMM contains memory-mapped registers (MMR) that control device-specific multiplexed functions.
 *  The safety and diagnostic features of the IOMM are:
 *  - Kicker mechanism to protect the MMRs from accidental writes
 *  - Master-id checker to only allow the CPU to write to the MMRs
 *  - Error indication for access violations
 *
 *  Related Files
 *   - reg_pinmux.h
 *   - pinmux.h
 *   - pinmux.c
 *  @addtogroup IOMM
 *  @{
 */

/** @fn void muxInit(void)
*   @brief Initializes the PINMUX Driver
*
*   This function initializes the PINMUX module and configures the selected
*   pinmux settings as per the user selection in the GUI
*/
void muxInit(void);
void pinmuxGetConfigValue(pinmux_config_reg_t *config_reg, config_value_type_t type);
/* USER CODE BEGIN (0) */
/* USER CODE END */

/**@}*/
#ifdef __cplusplus
}
#endif /*extern "C" */
#endif
