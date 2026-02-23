#ifndef L3GD20H_H_
#define L3GD20H_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    #define GYRO_RANGE                       245    // dps
    #if GYRO_RANGE == 245
        #define SPI_GYRO_RANGE                0          // SPI register set to 0
        #define GYRO_SENSITIVITY              0.00875f   // sensitivity for 245 dps range
    #elif GYRO_RANGE == 500
        #define SPI_GYRO_RANGE                1          // SPI register set to 1
        #define GYRO_SENSITIVITY              0.01750f   // sensitivity for 500 dps range
    #elif GYRO_RANGE == 2000
        #define SPI_GYRO_RANGE                2          // SPI register set to 2
        #define GYRO_SENSITIVITY              0.07000f   // sensitivity for 2000 dps range
    #endif

#define REG_WHO_AM_I     0x0F
#define REG_CTRL1        0x20
#define REG_CTRL2        0x21
#define REG_CTRL3        0x22
#define REG_CTRL4        0x23
#define REG_CTRL5        0x24
#define REG_REFERENCE    0x25
#define REG_OUT_TEMP     0x26
#define REG_STATUS       0x27
#define REG_OUT_X_L      0x28
#define REG_OUT_X_H      0x29
#define REG_OUT_Y_L      0x2A
#define REG_OUT_Y_H      0x2B
#define REG_OUT_Z_L      0x2C
#define REG_OUT_Z_H      0x2D
#define REG_FIFO_CTL     0x2E
#define REG_FIFO_SRC     0x2F
#define REG_IG_CFG       0x30
#define REG_IG_SRC       0x31
#define REG_IG_THS_XH    0x32
#define REG_IG_THS_XL    0x33
#define REG_IG_THS_YH    0x34
#define REG_IG_THS_YL    0x35
#define REG_IG_THS_ZH    0x36
#define REG_IG_THS_ZL    0x37
#define REG_IG_DURATION  0x38
#define REG_LOW_ODR      0x3


union Ctrl1 {
    uint8_t val;
    struct {
        uint8_t yen:1;
        uint8_t xen:1;
        uint8_t zen:1;
        uint8_t pd:1;
        uint8_t bw:2;
        uint8_t dr:2;
    } bits;
};

typedef enum Hpm{
    NORMAL_MODE = 0,
    REFERENCE_SIGNAL = 1,
    NORMAL_MODE_2 = 2,
    AUTORESET = 3
} Hpm_t;

union Ctrl2Reg{
    uint8_t val;
    struct {
        uint8_t hpcf:4;
        uint8_t hpm:2;
        uint8_t extRen:1;
    } bits;
};

union Ctrl3Reg{
    uint8_t val;
    struct {
        uint8_t int2Empty:1;
        uint8_t int2ORun:1;
        uint8_t int2Fth:1;
        uint8_t int2Drdy:1;
        uint8_t ppOd:1;
        uint8_t hLactive:1;
        uint8_t int1Boot:1;
        uint8_t int1Ig:1;
    } bits;
};

union Ctrl4Reg{
    uint8_t val;
    struct {
        uint8_t sim:1;
        uint8_t st:2;
        uint8_t imPen:1;
        uint8_t fs:2;
        uint8_t ble:1;
        uint8_t bdu:1;
    } bits;
};

union Ctrl5Reg{
    uint8_t val;
    struct {
        uint8_t outsel:2;
        uint8_t igsel:2;
        uint8_t hPen:2;
        uint8_t stopOnFth:1;
        uint8_t fifoEn:1;
        uint8_t boot:1;
    } bits;
};

union StatusReg{
    uint8_t val;
    struct {
        uint8_t xda:1;
        uint8_t yda:1;
        uint8_t zda:1;
        uint8_t zyxda:1;
        uint8_t xovr:1;
        uint8_t yovr:1;
        uint8_t zovr:1;
        uint8_t zyxovr:1;
    } bits;
};

typedef enum FifoMode{
  BYPASS = 0,
  FIFO = 1,
  STREAM = 2,
  STREAM_TO_FIFO = 3,
  BYPASS_TO_STREAM = 4,
  DYNAMIC_STREAM = 6,
  BYPASS_TO_FIFO =7
} FifoMode_t;

union FifoCtlReg{
    uint8_t val;
    struct {
        uint8_t fifo_thresh:5;
        uint8_t fifo_mode:3;
    } bits;
};


union LowOdrReg{
    uint8_t val;
    struct {
        uint8_t lowOdr:1;
        uint8_t rsv:1;
        uint8_t swRes:1;
        uint8_t i2cDis:1;
        uint8_t rsv2:1;
        uint8_t drdyHl:1;
        uint8_t rsv3:2;
    } bits;
};
    
typedef enum {
    GYRO_STATUS_OK = 0,
    GYRO_STATUS_ERR = 1
} gyro_status_t;

struct GyroVector {
    // Arrays in X, Y, Z order
    int16_t raw[3];
    float32 gyro[3];
};


gyro_status_t gyro_init();
gyro_status_t gyro_get_data(struct GyroVector *data);

    
#ifdef __cplusplus
}
#endif
    
#endif  // L3GD20H_H_
