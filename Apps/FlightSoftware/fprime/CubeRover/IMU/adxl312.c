#include <stdint.h>

#include <spi.h>


#include "adxl312.h"

#define RW_OFFSET       7
#define READ_BIT        1
#define WRITE_BIT       0
#define MB_OFFSET       6

/* Device ID */
static const uint8_t DEVID_EXPECTED = 0xE5;    //0xE5; //AD = adxl313, E5 = adxl312
static const uint8_t DEVID_TEST     = 0xAD;

/* SPI configuration */
static spiBASE_t *spi = spiREG3;
static spiDAT1_t dataConfig = {
    .CS_HOLD = 0,
    .WDEL = 0,
    .DFSEL = SPI_FMT_0,
    .CSNR = 0b11011111   // Each index corresponds to CS[i]. Value represents the CS level when a transaction is occurring (1: hign, 0: low). SPIDEF sets the non-transaction CS level.
                         // Note that SPI3 only has 6 CS pins, the upper 2 bits are don't care.
};

/* Store state globally */
union PowerMode g_adxl312_power_mode = {0};
union DataFormat g_adxl312_data_format = {0};
union BWRate g_adxl312_bw_rate = {0};

accel_status_t accel_init() {
    /* It is assumed the SPI module has already been initialized */
    uint8_t id = 0;
    id = read_register(REG_DEVID);
    if ((id != DEVID_EXPECTED) && (id != DEVID_TEST))
        return ACCEL_STATUS_ERR;

    g_adxl312_data_format.bits.range = range_3g;
    write_register(REG_DATA_FORMAT, g_adxl312_data_format.value);

    g_adxl312_bw_rate.bits.rate = ACCEL_100HZ;
    write_register(REG_BW_RATE, g_adxl312_bw_rate.value);

    g_adxl312_power_mode.bits.measure = 1;
    write_register(REG_POWER_CTL, g_adxl312_power_mode.value);

    uint8_t write_test = read_register(REG_POWER_CTL);
    if (write_test != g_adxl312_power_mode.value)
        return ACCEL_STATUS_ERR;

    return ACCEL_STATUS_OK;
}

accel_status_t accel_set_range(accel_range_t sampleRange) {
    write_register(REG_DATA_FORMAT, sampleRange);
    return ACCEL_STATUS_OK;
}

inline float32 lsb_to_acc(int16_t buf) {
    float32 scale;
    switch (g_adxl312_data_format.bits.range) {
    case range_1_5g:
        scale = 2.9e-3;
        break;
    case range_3g:
        scale = 5.8e-3;
        break;
    case range_6g:
        scale = 11.6e-3;
        break;
    case range_12g:
        scale = 23.2e-3;
        break;
    default:
        scale = 0.;
    }
    return (float32)buf * scale;
}

accel_status_t accel_get_data(struct AccVector *data) {
    read_multibyte(REG_DATA_START, 6, (uint8_t *)data->raw);

    /* RM46L852 is little-endian which matches ADXL312 acc-data register endianess. Do not need to swap bytes.
    data->raw[0] = swap2bytes(&data->raw[0]);
    data->raw[1] = swap2bytes(&data->raw[1]);
    data->raw[2] = swap2bytes(&data->raw[2]);
    */

    data->acc[0] = lsb_to_acc(data->raw[0]);
    data->acc[1] = lsb_to_acc(data->raw[1]);
    data->acc[2] = lsb_to_acc(data->raw[2]);
    
    // TODO: TIMESTAMP

    return ACCEL_STATUS_OK;
}

/* Reimplementation of HAL spiTransmitData since the HALCoGen version disables CSHold when transmitting a single byte */
static uint32 spiTransmitOneByte(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint16* srcbuff) {
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
static uint32 spiReceiveOneByte(spiBASE_t *spi, spiDAT1_t *dataconfig_t, uint16 * destbuff) {
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

void write_register(uint8_t addr, uint8_t value) {
    dataConfig.CS_HOLD = 1; /* CS_HOLD starts true to send next frame */

    /* Send the register address */
    uint16_t dataBuf = addr;
    spiTransmitOneByte(spi, &dataConfig, &dataBuf);

    /* Send the register value */
    dataConfig.CS_HOLD = 0; /* This is the last frame */
    dataBuf = value;
    spiTransmitOneByte(spi, &dataConfig, &dataBuf);
}

uint8_t read_register(uint8_t addr) {
    /* Send the register address */
    dataConfig.CS_HOLD = 1; /* CS_HOLD starts true to send next frame */
    uint16_t dataBuf = (READ_BIT << RW_OFFSET) | addr;
    spiTransmitOneByte(spi, &dataConfig, &dataBuf);

    /* Receive the register value */
    dataConfig.CS_HOLD = 0; /* This is the last frame */
    dataBuf = 0;
    spiReceiveOneByte(spi, &dataConfig, &dataBuf);

    return (uint8_t)(dataBuf & 0xFF);
}

void read_multibyte(uint8_t startAddr, uint8_t count, uint8_t *buf) {
    /* Send the starting register address */
    dataConfig.CS_HOLD = 1; /* CS_HOLD starts true to send next frame */
    uint16_t dataBuf = (READ_BIT << RW_OFFSET) | (1 << MB_OFFSET) | startAddr;
    spiTransmitOneByte(spi, &dataConfig, &dataBuf);

    uint8_t i;
    for (i = 0; i < count; ++i)
    {
        /* On the last frame, set CS_HOLD to false */
        if (i == count - 1)
        {
            dataConfig.CS_HOLD = 0;
        }

        spiReceiveOneByte(spi, &dataConfig, &dataBuf);
        buf[i] = (uint8_t)(dataBuf & 0xFF);
    }
}
