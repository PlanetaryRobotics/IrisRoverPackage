// ======================================================================
// \title   ImuComponentImpl.hpp
// \author  cedric
// \editted michael
// \brief   hpp file for Imu component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef IMU_HPP
#define IMU_HPP

#include <stdlib.h>
#include "CubeRover/IMU/IMUComponentAc.hpp"
#include "spi.h"
#include "gio.h"

#include "adxl312.h"

namespace CubeRover {

    typedef enum{
        IMU_NO_ERROR,
        IMU_UNEXPECTED_ERROR,
        IMU_WRONG_DATA_SIZE,
        IMU_MAX_ENUM_SIZE
    }IMUError;

    typedef uint8_t IMUI2cSlaveAddress;

    #define L3GD20H_DEVICE_ID             0xD7

    #define CS_SPIPORT3_BIT_L3GD20H       1

    #define ACCELEROMETER_RANGE           3   // g
    #if ACCELEROMETER_RANGE == 1
        #define SPI_ACC_RANGE                 0
        #define ACC_SENSITIVITY               0.0029f
    #elif ACCELEROMETER_RANGE == 3
        #define SPI_ACC_RANGE                 1
        #define ACC_SENSITIVITY               0.0058f
    #elif ACCELEROMETER_RANGE == 6
        #define SPI_ACC_RANGE                 2
        #define ACC_SENSITIVITY               0.0116f
    #elif ACCELEROMETER_RANGE == 12
        #define SPI_ACC_RANGE                 3
        #define ACC_SENSITIVITY               0.0232f
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

    #define ONE_OVER_PI                   0.3183098861837

    #define IMU_LPF_COEFF                 0.01
    #define MAX_ROVER_PITCH_ANGLE         40
    #define MAX_ROVER_ROLL_ANGLE          40

    enum SpiBufferSize{
        SPI_RX_BUFFER_SIZE=16,
        SPI_TX_BUFFER_SIZE=16
    };

    namespace L3gd20h{
        //L3GD20H register definition
        enum L3gd20hRegister{
            WHO_AM_I    = 0x0F,
            CTRL1       = 0x20,
            CTRL2       = 0x21,
            CTRL3       = 0x22,
            CTRL4       = 0x23,
            CTRL5       = 0x24,
            REFERENCE   = 0x25,
            OUT_TEMP    = 0x26,
            STATUS      = 0x27,
            OUT_X_L     = 0x28,
            OUT_X_H     = 0x29,
            OUT_Y_L     = 0x2A,
            OUT_Y_H     = 0x2B,
            OUT_Z_L     = 0x2C,
            OUT_Z_H     = 0x2D,
            FIFO_CTL    = 0x2E,
            FIFO_SRC    = 0x2F,
            IG_CFG      = 0x30,
            IG_SRC      = 0x31,
            IG_THS_XH   = 0x32,
            IG_THS_XL   = 0x33,
            IG_THS_YH   = 0x34,
            IG_THS_YL   = 0x35,
            IG_THS_ZH   = 0x36,
            IG_THS_ZL   = 0x37,
            IG_DURATION = 0x38,
            LOW_ODR     = 0x39
        };

        struct Ctl1Bits{
            uint8_t yen:1;
            uint8_t xen:1;
            uint8_t zen:1;
            uint8_t pd:1;
            uint8_t bw:2;
            uint8_t dr:2;
        };

        union Ctl1Reg{
            uint8_t all;
            Ctl1Bits bit;
        };

        typedef enum Hpm{
            NORMAL_MODE =0,
            REFERENCE_SIGNAL =1,
            NORMAL_MODE_2 =2,
            AUTORESET =3
        }Hpm;

        struct Ctl2Bits{
            uint8_t hpcf:4;
            uint8_t hpm:2;
            uint8_t extRen:1;
        };

        union Ctl2Reg{
            uint8_t all;
            Ctl2Bits bit;
        };

        struct Ctl3Bits{
            uint8_t int2Empty:1;
            uint8_t int2ORun:1;
            uint8_t int2Fth:1;
            uint8_t int2Drdy:1;
            uint8_t ppOd:1;
            uint8_t hLactive:1;
            uint8_t int1Boot:1;
            uint8_t int1Ig:1;
        };

        union Ctl3Reg{
            uint8_t all;
            Ctl3Bits bit;
        };

        struct Ctl4Bits{
            uint8_t sim:1;
            uint8_t st:2;
            uint8_t imPen:1;
            uint8_t fs:2;
            uint8_t ble:1;
            uint8_t bdu:1;
        };

        union Ctl4Reg{
            uint8_t all;
            Ctl4Bits bit;
        };

        struct Ctl5Bits{
            uint8_t outsel:2;
            uint8_t igsel:2;
            uint8_t hPen:2;
            uint8_t stopOnFth:1;
            uint8_t fifoEn:1;
            uint8_t boot:1;
        };

        union Ctl5Reg{
            uint8_t all;
            Ctl5Bits bit;
        };

        struct StatusRegBits{
            uint8_t xda:1;
            uint8_t yda:1;
            uint8_t zda:1;
            uint8_t zyxda:1;
            uint8_t xovr:1;
            uint8_t yovr:1;
            uint8_t zovr:1;
            uint8_t zyxovr:1;
        };

        union StatusReg{
            uint8_t all;
            StatusRegBits bit;
        };

        typedef enum FifoMode{
          BYPASS = 0,
          FIFO = 1,
          STREAM = 2,
          STREAM_TO_FIFO = 3,
          BYPASS_TO_STREAM = 4,
          DYNAMIC_STREAM = 6,
          BYPASS_TO_FIFO =7
        }FifoMode;

        struct FifoCtlRegBits{
            uint8_t fifo_thresh:5;
            uint8_t fifo_mode:3;
        };

        union FifoCtlReg{
            uint8_t all;
            FifoCtlRegBits bit;
        };

        struct LowOdrRegBits{
            uint8_t lowOdr:1;
            uint8_t rsv:1;
            uint8_t swRes:1;
            uint8_t i2cDis:1;
            uint8_t rsv2:1;
            uint8_t drdyHl:1;
            uint8_t rsv3:2;
        };

        union LowOdrReg{
            uint8_t all;
            LowOdrRegBits bit;
        };
    } // end of namespace L3gd20h

    class IMUComponentImpl :
      public IMUComponentBase
    {
    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object Imu
      //!
      IMUComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object Imu
      //!
      void init(
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object Imu
      //!
      ~IMUComponentImpl(void);

      IMUError setupGyroscope(spiBASE_t *spi);

      void computePitchRoll(float32 *pitch, float32 *roll);

      IMUError gyroReadData(const L3gd20h::L3gd20hRegister regStartAddr, uint16_t *rxData, const uint8_t length);
      IMUError gyroWriteData(const L3gd20h::L3gd20hRegister regStartAddr, uint16_t *txData, const uint8_t length);
      IMUError readAngularRates(float32 *gyrX, float32 *gyrY,  float32 *gyrZ);

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                           NATIVE_UINT_TYPE context /*!< The call order*/);

      //! Handler implementation for PingIn
      //!
      void PingIn_handler(const NATIVE_INT_TYPE portNum, /*!< The port number*/
                          U32 key /*!< Value to return to pinger*/);


      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for Imu_ReportData command handler
      //! A command to force an IMU data.
      void IMU_ReportData_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                     const U32 cmdSeq /*!< The command sequence number*/);

      //! Implementation for IMU_TurnOn command handler
      //! A command to turn on the hardware
      void IMU_TurnOn_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                 const U32 cmdSeq /*!< The command sequence number*/);

      //! Implementation for IMU_TurnOff command handler
      //! A command to turn off the hardware
      void IMU_TurnOff_cmdHandler(const FwOpcodeType opCode, /*!< The opcode*/
                                  const U32 cmdSeq /*!< The command sequence number*/);

    private:
      spiBASE_t *m_spi;     // Depracate and DELETE this
      struct AccVector m_acc;
      uint16_t m_spiRxBuff[SPI_RX_BUFFER_SIZE];
      uint16_t m_spiTxBuff[SPI_TX_BUFFER_SIZE];
      spiDAT1_t m_gyroDataSingleByteConfig;
      spiDAT1_t m_gyroDataDoubleByteConfig;
      float32 m_lpfAccX;
      float32 m_lpfAccY;
      float32 m_lpfAccZ;
    };

} // end namespace CubeRover

#endif
