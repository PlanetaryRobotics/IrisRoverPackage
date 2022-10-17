#include "SPI.h"

/* Reimplementation of HAL spiTransmitData since the HALCoGen version disables CSHold when transmitting a single byte */
uint32 spiTransmitOneByte(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint16* srcbuff) {
    volatile uint32 SpiBuf;
    uint16 Tx_Data;
    uint32 Chip_Select_Hold = (dataconfig_t->CS_HOLD) ? 0x10000000U : 0U;
    uint32 WDelay = (dataconfig_t->WDEL) ? 0x04000000U : 0U;
    SPIDATAFMT_t DataFormat = dataconfig_t->DFSEL;
    uint8 ChipSelect = dataconfig_t->CSNR;

    if ((spi->FLG & 0x000000FFU) !=0U) {
        return (spi->FLG & 0xFFU);
    }

    /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
    Tx_Data = *srcbuff;

    spi->DAT1 =  ((uint32)DataFormat << 24U) |
                 ((uint32)ChipSelect << 16U) |
                 (WDelay)           |
                 (Chip_Select_Hold) |
                 (uint32)Tx_Data;
    /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
    while((spi->FLG & 0x00000100U) != 0x00000100U);
    SpiBuf = spi->BUF;

    return (spi->FLG & 0xFFU);
}

/* Reimplementation of HAL spiReceiveData since the HALCoGen version disables CSHold when receiving a single byte */
uint32 spiReceiveOneByte(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint16 *destbuff) {
    uint32 Chip_Select_Hold = (dataconfig_t->CS_HOLD) ? 0x10000000U : 0U;
    uint32 WDelay = (dataconfig_t->WDEL) ? 0x04000000U : 0U;
    SPIDATAFMT_t DataFormat = dataconfig_t->DFSEL;
    uint8 ChipSelect = dataconfig_t->CSNR;

    if((spi->FLG & 0x000000FFU) !=0U) {
      return (spi->FLG & 0xFFU);
    }

    /*SAFETYMCUSW 51 S MR:12.3 <APPROVED> "Needs shifting for 32-bit value" */
    spi->DAT1 = ((uint32)DataFormat << 24U) |
                ((uint32)ChipSelect << 16U) |
                (WDelay)            |
                (Chip_Select_Hold)  |
                (0x00000000U);
    /*SAFETYMCUSW 28 D MR:NA <APPROVED> "Hardware status bit read check" */
    while((spi->FLG & 0x00000100U) != 0x00000100U);
    /*SAFETYMCUSW 45 D MR:21.1 <APPROVED> "Valid non NULL input parameters are only allowed in this driver" */
    *destbuff = (uint16)spi->BUF;

    return (spi->FLG & 0xFFU);
}

void write_register(spiBASE_t *spi, spiDAT1_t *dataConfig, uint8 addr, uint8 value) {
    dataConfig->CS_HOLD = 1; /* CS_HOLD starts true to send next frame */

    /* Send the register address */
    uint16 dataBuf = (WRITE_BIT << RW_OFFSET) | addr;
    spiTransmitOneByte(spi, dataConfig, &dataBuf);

    /* Send the register value */
    dataConfig->CS_HOLD = 0; /* This is the last frame */
    dataBuf = value;
    spiTransmitOneByte(spi, dataConfig, &dataBuf);
}

uint8 read_register(spiBASE_t *spi, spiDAT1_t *dataConfig, uint8 addr) {
    /* Send the register address */
    dataConfig->CS_HOLD = 1; /* CS_HOLD starts true to send next frame */
    uint16 dataBuf = (READ_BIT << RW_OFFSET) | addr;
    spiTransmitOneByte(spi, dataConfig, &dataBuf);

    /* Receive the register value */
    dataConfig->CS_HOLD = 0; /* This is the last frame */
    dataBuf = 0;
    spiReceiveOneByte(spi, dataConfig, &dataBuf);

    return (uint8)(dataBuf & 0xFF);
}

void read_multibyte(spiBASE_t *spi, spiDAT1_t *dataConfig, uint8 startAddr, uint8 count, uint8 *buf) {
    /* Send the starting register address */
    dataConfig->CS_HOLD = 1; /* CS_HOLD starts true to send next frame */
    uint16 dataBuf = (READ_BIT << RW_OFFSET) | (1 << MB_OFFSET) | startAddr;
    spiTransmitOneByte(spi, dataConfig, &dataBuf);

    uint8 i;
    for (i = 0; i < count; ++i) {
        /* On the last frame, set CS_HOLD to false */
        if (i == count - 1) {
            dataConfig->CS_HOLD = 0;
        }

        spiReceiveOneByte(spi, dataConfig, &dataBuf);
        buf[i] = (uint8)(dataBuf & 0xFF);
    }
}
