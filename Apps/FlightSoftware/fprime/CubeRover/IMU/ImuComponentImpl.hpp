// ======================================================================
// \title  ImuComponentImpl.hpp
// \author cedric
// \brief  hpp file for Imu component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef Imu_HPP
#define Imu_HPP

#include <stdlib.h>
#include "CubeRover/IMU/IMUComponentAc.hpp"
#include "spi.h"
#include "gio.h"

namespace CubeRover {

  typedef enum{
    IMU_NO_ERROR,
    IMU_UNEXPECTED_ERROR,
    IMU_WRONG_DATA_SIZE,
    IMU_MAX_ENUM_SIZE
  }ImuError;

  typedef uint8_t ImuI2cSlaveAddress;
  #define SET_ADXL_SPI_WRITE_BIT(x) (x & ~(0x01 << 7))
  #define SET_ADXL_SPI_READ_BIT(x) (x | (0x01 << 7))
  #define SET_ADXL_SPI_SINGLETRANS(x) (x & ~(0x01 << 6))
  #define SET_ADXL_SPI_MULTITRANS(x) (x | (0x01 << 6))
  #define ADXL_DEVICE_ID      0xE5

  class ImuComponentImpl :
    public ImuComponentBase
  {
    enum SpiBufferSize{
        SPI_RX_BUFFER_SIZE=16,
        SPI_TX_BUFFER_SIZE=16
    };

    enum AdxlRegister{
        DEVICE_ID       = 0x00,
        OFFSET_X        = 0x1E,
        OFFSET_Y        = 0x1F,
        OFFSET_Z        = 0x20,
        THRESH_ACT      = 0x24,
        THRESH_INACT    = 0x25,
        TIME_INACT      = 0x26,
        ACT_INACT_CTL   = 0x27,
        BW_RATE         = 0x2C,
        POWER_CTL       = 0x2D,
        INT_ENABLE      = 0x2E,
        INT_MAP         = 0x2F,
        INT_SOURCE      = 0x30,
        DATA_FORMAT     = 0x31,
        DATAX0          = 0x32,
        DATAX1          = 0x33,
        DATAY0          = 0x34,
        DATAY1          = 0x35,
        DATAZ0          = 0x36,
        DATAZ1          = 0x37,
        FIFO_CTL        = 0x38,
        FIFO_STATUS     = 0x39
    };

    struct ActInactCtlBits{
        uint8_t INACT_Z_enable:1;
        uint8_t INACT_Y_enable:1;
        uint8_t INACT_X_enable:1;
        uint8_t INACT_ac_dc:1;
        uint8_t ACT_Z_enable:1;
        uint8_t ACT_Y_enable:1;
        uint8_t ACT_X_enable:1;
        uint8_t ACT_ac_dc:1;
    };

    union ActInactCtlReg{
        uint8_t all;
        ActInactCtlBits bit;
    };

    struct BwRateBits{
        uint8_t rate:4;
        uint8_t low_power:1;
        uint8_t rsv:3;
    };

    union BwRateReg{
        uint8_t all;
        BwRateBits bit;
    };

    typedef enum WakeupBits{
        WAKE_UP_8HZ=0,
        WAKE_UP_4HZ=1,
        WAKE_UP_2HZ=2,
        WAKE_UP_1HZ=3,
    }WakeupBits;

    struct PowerCtlBits{
        uint8_t wakeup:2;
        uint8_t sleep:1;
        uint8_t measure:1;
        uint8_t auto_sleep:1;
        uint8_t link:1;
        uint8_t rsv:2;
    };

    union PowerCtlReg{
        uint8_t all;
        PowerCtlBits bit;
    };

    struct IntRegBits{
        uint8_t overrun:1;
        uint8_t watermark:1;
        uint8_t rsv:1;
        uint8_t inactivity:1;
        uint8_t activity:1;
        uint8_t rsv2:1;
        uint8_t data_ready:1;
    };

    union IntReg{
        uint8_t all;
        IntRegBits bit;
    };

    union IntMapReg{
        uint8_t all;
        IntRegBits bit;
    };

    union IntSrcReg{
        uint8_t all;
        IntRegBits bit;
    };

    typedef enum DataFormatRange{
        RANGE_1_5G = 0b00,
        RANGE_3G = 0b01,
        RANGE_6G = 0b10,
        RANGE_12G = 0b11
    }DataFormatRange;

    struct DataFormatBits{
        uint8_t range:2;
        uint8_t justify:1;
        uint8_t fullRes:1;
        uint8_t rsv:1;
        uint8_t int_invert:1;
        uint8_t spi:1;
        uint8_t self_test:1;
    };

    union DataFormatReg{
        uint8_t all;
        DataFormatBits bit;
    };

    typedef enum FifoMode{
        BYPASS=0,
        FIFO=1,
        STREAM=2,
        TRIGGER=3
    }FifoMode;

    struct FifoCtlBits{
        uint8_t samples:5;
        uint8_t trigger:1;
        uint8_t fifo_mode:2;
    };

    union FifoCtlReg{
        uint8_t all;
        FifoCtlBits bit;
    };

    struct FifoStsBits{
        uint8_t entries:6;
        uint8_t rsv:1;
        uint8_t fifo_trig:1;
    };

    union FIFO_STS{
        uint8_t all;
        FifoStsBits bit;
    };

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object Imu
      //!
      ImuComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object Imu
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object Imu
      //!
      ~ImuComponentImpl(void);

      ImuError setup(spiBASE_t *spi);
      ImuError readAccelerations(float32 *accX, float32 *accY,  float32 *accZ);
      ImuError accWriteData(const AdxlRegister regStartAddr, uint16_t *txData, const uint8_t length);
      ImuError accReadData(const AdxlRegister regStartAddr, uint16_t *rxData, const uint8_t length);

    PRIVATE:

      // ----------------------------------------------------------------------
      // Handler implementations for user-defined typed input ports
      // ----------------------------------------------------------------------

      //! Handler implementation for schedIn
      //!
      void schedIn_handler(
          const NATIVE_INT_TYPE portNum, /*!< The port number*/
          NATIVE_UINT_TYPE context /*!< The call order*/
      );

    PRIVATE:

      // ----------------------------------------------------------------------
      // Command handler implementations
      // ----------------------------------------------------------------------

      //! Implementation for Imu_ReportData command handler
      //! A command to force an IMU data.
      void Imu_ReportData_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );

    private:
      bool m_setup;
      spiBASE_t *m_spi;
      uint16_t m_spiRxBuff[SPI_RX_BUFFER_SIZE];
      uint16_t m_spiTxBuff[SPI_TX_BUFFER_SIZE];
      spiDAT1_t m_gyroDataConfig;
      spiDAT1_t m_accDataConfig;
    };

} // end namespace CubeRover

#endif
