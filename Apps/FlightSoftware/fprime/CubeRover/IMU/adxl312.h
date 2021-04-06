#ifndef ADXL312_H_
#define ADXL312_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
    
/* Register Addresses */
#define REG_DEVID       0x00
#define REG_BW_RATE     0x2C
#define REG_POWER_CTL   0x2D
#define REG_DATA_FORMAT 0x31
#define REG_DATAX0      0x32
#define REG_DATAX1      0x33
#define REG_DATAY0      0x34
#define REG_DATAY1      0x35
#define REG_DATAZ0      0x36
#define REG_DATAZ1      0x37
#define REG_DATA_START  0x32

typedef enum {
    ACCEL_STATUS_OK = 0,
    ACCEL_STATUS_ERR = 1
} accel_status_t;

union PowerMode {
    struct {
        uint8_t wakeup      : 2;
        uint8_t sleep       : 1;
        uint8_t measure     : 1;
        uint8_t auto_sleep  : 1;
        uint8_t link        : 1;
        uint8_t reserved    : 2;
    } bits;
    uint8_t value;
};

typedef enum {
    range_1_5g = 0,
    range_3g = 1,
    range_6g = 2,
    range_12g = 3
} accel_range_t;

union DataFormat {
    struct {
        uint8_t range       : 2;
        uint8_t justify     : 1;
        uint8_t full_res    : 1;
        uint8_t reserved    : 1;
        uint8_t int_invert  : 1;
        uint8_t spi         : 1;
        uint8_t self_test   : 1;
    } bits;
    uint8_t value;
};

typedef enum {
    ACCEL_6_25HZ = 0b0110,
    ACCEL_12_5HZ = 0b0111,
    ACCEL_25HZ   = 0b1000,
    ACCEL_50HZ   = 0b1001,
    ACCEL_100HZ  = 0b1010,
    ACCEL_200HZ  = 0b1011,
    ACCEL_400HZ  = 0b1100,
    ACCEL_800HZ  = 0b1101,
    ACCEL_1600HZ = 0b1110,
    ACCEL_3200HZ = 0b1111
} accel_data_rate_t;

union BWRate {
    struct {
        uint8_t rate      : 4;
        uint8_t low_power : 1;
        uint8_t reserved  : 3;
    } bits;
    uint8_t value;
};

struct AccVector {
    // Arrays in X, Y, Z order
    int16_t raw[3];
    float32 acc[3];
};

accel_status_t accel_init();
accel_status_t accel_set_range(accel_range_t sampleRange);
accel_status_t accel_get_data(struct AccVector *data);

#ifdef __cplusplus
}
#endif

#endif /* ADXL312_H_ */
