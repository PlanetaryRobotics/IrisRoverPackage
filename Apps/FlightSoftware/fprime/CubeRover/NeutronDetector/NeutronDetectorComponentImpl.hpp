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
#include "spi.h"
#include "gio.h"

#include "CubeRoverConfig.hpp"

namespace CubeRover {
  namespace NeutronDetector{
    #define TOTAL_MSND_PLATE            5
    #define TOTAL_MUX_SENSOR_SELECT     4
    #define TOTAL_MSND_PER_PLATE        16
    #define MSND_TX_PACKET_SIZE_BYTE    1
    #define MSND_RX_PACKET_SIZE_BYTE    1
    #define SPI_REG_MSND                spiREG3

    typedef uint8_t NeutronSensorData;
    typedef NeutronSensorData * NeutronSensorArray;

    typedef enum Error{
      ND_NO_ERROR = 0
    }NeutronError;

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
      NeutronDetector::Error setMultiplexer(const uint16_t sensor, const uint16_t sensorPlate);
      NeutronDetector::Error spiReadSensorData(NeutronDetector::NeutronSensorData *data);
      void mapMuxOutputs();

    private:
      NeutronDetector::NeutronSensorData m_neutronSensorArray[TOTAL_MSND_PLATE*TOTAL_MSND_PER_PLATE];
      NeutronDetector::MuxPortMap m_muxPlateSelect[TOTAL_MSND_PLATE];
      NeutronDetector::MuxPortMap m_muxSensorSelect[TOTAL_MUX_SENSOR_SELECT];

      spiDAT1_t m_msndDataConfig;
      spiBASE_t *m_msndSpi;
      uint8_t m_spiTxBuff[MSND_TX_PACKET_SIZE_BYTE];
      uint8_t m_spiRxBuff[MSND_RX_PACKET_SIZE_BYTE];

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
