#include <stdint.h>

#include <spi.h>
#include "App/SPI.h"

#include "adxl312.h"

/* Device ID */
#define DEVID_EXPECTED 0xE5    //0xE5; //AD = adxl313, E5 = adxl312
#define DEVID_TEST     0xAD

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
    id = read_register(spi, &dataConfig, REG_DEVID);
    if ((id != DEVID_EXPECTED) && (id != DEVID_TEST))
        return ACCEL_STATUS_ERR;

    g_adxl312_data_format.bits.range = range_3g;
    write_register(spi, &dataConfig, REG_DATA_FORMAT, g_adxl312_data_format.value);

    g_adxl312_bw_rate.bits.rate = ACCEL_100HZ;
    write_register(spi, &dataConfig, REG_BW_RATE, g_adxl312_bw_rate.value);

    g_adxl312_power_mode.bits.measure = 1;
    write_register(spi, &dataConfig, REG_POWER_CTL, g_adxl312_power_mode.value);

    uint8_t write_test = read_register(spi, &dataConfig, REG_POWER_CTL);
    if (write_test != g_adxl312_power_mode.value)
        return ACCEL_STATUS_ERR;

    return ACCEL_STATUS_OK;
}

accel_status_t accel_set_range(accel_range_t sampleRange) {
    write_register(spi, &dataConfig, REG_DATA_FORMAT, sampleRange);
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
    read_multibyte(spi, &dataConfig, REG_DATA_START, 6, (uint8_t *)data->raw);

    /* RM46L852 is little-endian which matches ADXL312 acc-data register endianess. Do not need to swap bytes.
    data->raw[0] = swap2bytes(&data->raw[0]);
    data->raw[1] = swap2bytes(&data->raw[1]);
    data->raw[2] = swap2bytes(&data->raw[2]);
    */

    data->acc[0] = lsb_to_acc(data->raw[0]);
    data->acc[1] = lsb_to_acc(data->raw[1]);
    data->acc[2] = lsb_to_acc(data->raw[2]);
    
    return ACCEL_STATUS_OK;
}
