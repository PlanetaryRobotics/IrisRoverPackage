// ======================================================================
// \title  NeutronDetectorComponentImpl.hpp
// \author cedric
// \brief  hpp file for NeutronDetector component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================

#ifndef NeutronDetector_HPP
#define NeutronDetector_HPP
#include <string.h>
#include <stdlib.h>

#include "CubeRover/NeutronDetector/NeutronDetectorComponentAc.hpp"

// include required peripherals
#include "gio.h"
#include "spi.h"

#include "etpwm.h"
#include "sys_core.h"

#include "CubeRoverConfig.hpp"

namespace CubeRover {
  namespace NeutronDetector{
    #define TOTAL_MSND_PLATE            5
    #define TOTAL_MUX_SENSOR_SELECT     4
    #define TOTAL_MSND_PER_PLATE        16
    #define MSND_TX_PACKET_SIZE_BYTE    1
    #define MSND_RX_PACKET_SIZE_BYTE    1
    #define TIMER_EPWM_REG              etpwmREG6

    #define SPI_REG_PORT                spiPORT3
    #define SPI_REG                     spiREG3
    #define SPI3_CS_BIT                 1
    #define SPI_TX_MAX_PACKET_SIZE_BYTE 3
    #define SPI_RX_MAX_PACKET_SIZE_BYTE 3
    #define GIO_DEV_PREFIX_ADDRESS      0x40
    #define GIO_DEV_ADDRESS             0x00

    typedef uint8_t NeutronSensorData;
    typedef NeutronSensorData * NeutronSensorArray;

    typedef uint16_t SensorIterator;
    typedef uint16_t SensorPlateIterator;

    typedef enum Error{
      ND_NO_ERROR = 0,
      ND_DATA_SIZE_ERROR = -1,
    }NeutronError;

    typedef enum IoExpanderRegAddress{
        IODIRA      = 0x00,
        IPOLA       = 0x02,
        GPINTENA    = 0x04,
        GPPUA       = 0x0C,
        GPIOA       = 0x12,
        OLATA       = 0x14,
        IODIRB      = 0x01,
        IPOLB       = 0x03,
        GPINTENB    = 0x05,
        GPPUB       = 0x0D,
        GPIOB       = 0x13,
        OLATB       = 0x15
    }IoExpanderRegAddress;

    struct IoconRegBits{
        uint8_t rsv:1;
        uint8_t intPol:1;
        uint8_t odr:1;
        uint8_t haen:1;
        uint8_t disslw:1;
        uint8_t seqop:1;
        uint8_t mirror:1;
        uint8_t bank:1;
    };

    typedef union Ioconreg{
        uint8_t all;
        IoconRegBits bit;
    }Ioconreg;

    typedef struct MuxPortMap{
        gioPORT_t *port;
        uint32_t bit;
    }MuxPortMap;

  } // end of namespace NeutronDetector

  class NeutronDetectorComponentImpl :
    public NeutronDetectorComponentBase
  {

    public:

      // ----------------------------------------------------------------------
      // Construction, initialization, and destruction
      // ----------------------------------------------------------------------

      //! Construct object NeutronDetector
      //!
      NeutronDetectorComponentImpl(
#if FW_OBJECT_NAMES == 1
          const char *const compName /*!< The component name*/
#else
          void
#endif
      );

      //! Initialize object NeutronDetector
      //!
      void init(
          const NATIVE_INT_TYPE queueDepth, /*!< The queue depth*/
          const NATIVE_INT_TYPE instance = 0 /*!< The instance number*/
      );

      //! Destroy object NeutronDetector
      //!
      ~NeutronDetectorComponentImpl(void);

      NeutronDetector::Error setupDetector();
      NeutronDetector::Error getSensorArray(NeutronDetector::NeutronSensorArray array);

    private:
      NeutronDetector::Error resetMultiplexer();
      NeutronDetector::Error setMultiplexer(const uint16_t sensor, const uint16_t sensorPlate);
      NeutronDetector::Error readSensorData(NeutronDetector::NeutronSensorData *data);
      NeutronDetector::Error spiWriteRegister(const NeutronDetector::IoExpanderRegAddress addr, const uint8_t val);
      NeutronDetector::Error spiReadRegister(const NeutronDetector::IoExpanderRegAddress addr, uint8_t *val, const uint8_t sizeOfData);
      NeutronDetector::Error setupGioExpander();
      void mapMuxOutputs();

    private:
      NeutronDetector::NeutronSensorData m_neutronSensorArray[TOTAL_MSND_PLATE*TOTAL_MSND_PER_PLATE];
      NeutronDetector::MuxPortMap m_muxPlateSelect[TOTAL_MSND_PLATE];
      NeutronDetector::MuxPortMap m_muxSensorSelect[TOTAL_MUX_SENSOR_SELECT];

      uint8_t m_msndBuff[MSND_RX_PACKET_SIZE_BYTE];
      uint16_t m_spiTxBuff[SPI_TX_MAX_PACKET_SIZE_BYTE];
      uint16_t m_spiRxBuff[SPI_RX_MAX_PACKET_SIZE_BYTE];
      uint8_t m_decoderLookUpTable[TOTAL_MSND_PER_PLATE];
      uint8_t m_plateLookUpTable[TOTAL_MSND_PLATE];

      spiDAT1_t m_spiDataConfigHandler;

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

      //! Implementation for ND_CollectData command handler
      //! Collect the data on asynchronous command
      void ND_CollectData_cmdHandler(
          const FwOpcodeType opCode, /*!< The opcode*/
          const U32 cmdSeq /*!< The command sequence number*/
      );


    };

} // end namespace CubeRover

#endif
