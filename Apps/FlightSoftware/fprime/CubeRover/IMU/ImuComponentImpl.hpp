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
  #define SET_ADXL_SPI_READ_BIT(x) (x & ~(0x01 << 7))
  #define SET_ADXL_SPI_WRITE_BIT(x) (x | (0x01 << 7))
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
