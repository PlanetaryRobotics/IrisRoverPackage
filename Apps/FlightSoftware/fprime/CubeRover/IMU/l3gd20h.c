#include <stdint.h>
#include <spi.h>

#include "App/SPI.h"

#include "l3gd20h.h"

#define L3GD20H_DEVICE_ID             0xD7


/* SPI configuration */
static spiBASE_t *spi = spiREG3;
static spiDAT1_t dataConfig = {
    .CS_HOLD = 0,
    .WDEL = 0,
    .DFSEL = SPI_FMT_0,
    .CSNR = 0b11111110   // Each index corresponds to CS[i]. Value represents the CS level when a transaction is occurring (1: hign, 0: low). SPIDEF sets the non-transaction CS level.
                         // Note that SPI3 only has 6 CS pins, the upper 2 bits are don't care.
};


gyro_status_t gyro_init() {
    if (read_register(spi, &dataConfig, REG_WHO_AM_I) != L3GD20H_DEVICE_ID)
        return GYRO_STATUS_ERR;

    union FifoCtlReg fifoReg = {.bits.fifo_mode = BYPASS};
    write_register(spi, &dataConfig, REG_FIFO_CTL, fifoReg.val);

    union Ctrl1 ctrl1Reg = {.bits.pd = 1, .bits.xen = 1, .bits.yen = 1, .bits.zen = 1};
    write_register(spi, &dataConfig, REG_CTRL1, ctrl1Reg.val);

    union Ctrl4Reg ctrl4Reg = {.bits.fs = SPI_GYRO_RANGE};
    write_register(spi, &dataConfig, REG_CTRL4, ctrl4Reg.val);

    return GYRO_STATUS_OK;
}

/* TODO: Implement similar global range setting for Gyro
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
*/

gyro_status_t gyro_get_data(struct GyroVector *data) {
    // read all angular rates data for x y z axis
    read_multibyte(spi, &dataConfig, REG_OUT_X_L, 6, (uint8_t *)data->raw);

    // CHECK THIS FOR GYRO
    /* RM46L852 is little-endian which matches ADXL312 acc-data register endianess. Do not need to swap bytes.
    data->raw[0] = swap2bytes(&data->raw[0]);
    data->raw[1] = swap2bytes(&data->raw[1]);
    data->raw[2] = swap2bytes(&data->raw[2]);
    */

    data->gyro[0] = (float32)(data->raw[0]) * GYRO_SENSITIVITY;
    data->gyro[1] = (float32)(data->raw[1]) * GYRO_SENSITIVITY;
    data->gyro[2] = (float32)(data->raw[2]) * GYRO_SENSITIVITY;
    
    return GYRO_STATUS_OK;
}
