// ======================================================================
// \title  NeutronDetectorComponentImpl.cpp
// \author cedric
// \brief  cpp file for NeutronDetector component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/NeutronDetector/NeutronDetectorComponentImpl.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  NeutronDetectorComponentImpl ::
#if FW_OBJECT_NAMES == 1
    NeutronDetectorComponentImpl(
        const char *const compName
    ) :
      NeutronDetectorComponentBase(compName)
#else
    NeutronDetectorComponentImpl(void)
#endif
  {
      m_msndSpi = NULL;
  }


  /**
   * @brief      Initialize the f-prime component
   *
   * @param[in]  queueDepth  The queue depth
   * @param[in]  instance    The instance
   */
  void NeutronDetectorComponentImpl ::
    init(
        const NATIVE_INT_TYPE queueDepth,
        const NATIVE_INT_TYPE instance
    )
  {
    NeutronDetectorComponentBase::init(queueDepth, instance);
  }


  /**
   * @brief      Destroys the object.
   */
  NeutronDetectorComponentImpl ::
    ~NeutronDetectorComponentImpl(void)
  {

  }


  /**
   * @brief      Sets the multiplexer.
   *
   * @param[in]  sensor       The sensor
   * @param[in]  sensorPlate  The sensor plate
   *
   * @return     The neutron detector error
   */
  NeutronDetector::Error NeutronDetectorComponentImpl :: setMultiplexer(const uint16_t sensor,
                                                                        const uint16_t sensorPlate){
    uint8_t i;
    // Plate selection is done by activating one output at a time
    // First disable all outputs
    for(i=0; i< TOTAL_MSND_PLATE; i++){
      gioSetBit(m_muxPlateSelect[i].port, m_muxPlateSelect[i].bit, 0);
      gioSetBit(m_muxSensorSelect[i].port, m_muxSensorSelect[i].bit, 0);      
    }

    // Then activate desired plate
    gioSetBit(m_muxPlateSelect[sensorPlate].port, m_muxPlateSelect[sensorPlate].bit, 1);

    // Select desired sensor
    for(i=0; i< TOTAL_MUX_SENSOR_SELECT; i++){
      // Set required bits 
      gioSetBit(m_muxSensorSelect[i].port, m_muxSensorSelect[i].bit, sensor >> i & 0x01b); 
    }

    return NeutronDetector::ND_NO_ERROR;
  }


  /**
   * @brief      Map the multiplexed outputs
   */
  void NeutronDetectorComponentImpl :: mapMuxOutputs(){
      // Map outputs that control the plate selection
      m_muxPlateSelect[0].port = gioPORTA;
      m_muxPlateSelect[0].bit = 0;

      m_muxPlateSelect[1].port = gioPORTA;
      m_muxPlateSelect[1].bit = 1;

      m_muxPlateSelect[2].port = gioPORTA;
      m_muxPlateSelect[2].bit = 2;

      m_muxPlateSelect[3].port = gioPORTA;
      m_muxPlateSelect[3].bit = 3;

      m_muxPlateSelect[4].port = gioPORTA;
      m_muxPlateSelect[4].bit = 4;

      // Map outputs that control the sensor selection
      m_muxSensorSelect[0].port = gioPORTA;
      m_muxSensorSelect[0].bit = 5;

      m_muxSensorSelect[1].port = gioPORTA;
      m_muxSensorSelect[1].bit = 6;

      m_muxSensorSelect[2].port = gioPORTA;
      m_muxSensorSelect[2].bit = 7;

      m_muxSensorSelect[3].port = gioPORTA;
      m_muxSensorSelect[3].bit = 8;
  }


  /**
   * @brief      Setup the detector
   *
   * @return     The neutron detector error
   */
  NeutronDetector::Error NeutronDetectorComponentImpl ::setupDetector(){
    mapMuxOutputs();

    m_msndSpi = SPI_REG_MSND;
    // SPI data configuration
    m_msndDataConfig.CS_HOLD = false;
    m_msndDataConfig.DFSEL = SPI_FMT_0;
    m_msndDataConfig.WDEL = false;

    return NeutronDetector::ND_NO_ERROR;
  }
  

  /**
   * @brief      Gets the sensor array.
   *
   * @param[in]  array  The array
   *
   * @return     The sensor array.
   */
  NeutronDetector::Error NeutronDetectorComponentImpl ::
    getSensorArray(NeutronDetector::NeutronSensorArray array){

    uint16_t sensor;
    uint16_t sensorPlate;

    // Scan through each sensor
    for(sensorPlate = 0; sensorPlate < TOTAL_MSND_PLATE; sensorPlate++){
     for(sensor = 0; sensor < TOTAL_MSND_PER_PLATE; sensor++){
        // Set the multiplexer to address the correct sensor
        setMultiplexer(sensorPlate, sensor);

        // Read the data from a single sensor and store it at the correct location
        spiReadSensorData(m_neutronSensorArray + sensor + sensorPlate*TOTAL_MSND_PLATE);
     }
   }
    return NeutronDetector::ND_NO_ERROR;
  }


  /**
   * @brief      Read sensor data over SPI
   *
   * @param      data  The data
   *
   * @return     The Neutron sensor detector
   */
  NeutronDetector::Error NeutronDetectorComponentImpl ::
    spiReadSensorData(NeutronDetector::NeutronSensorData *data){

    uint16_t totalBytesToTransmit = MSND_TX_PACKET_SIZE_BYTE;
    uint16_t i;

    // Set CS low
    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_NEUTRON_DETECT, 0);

    spiTransmitAndReceiveData(m_msndSpi,                /* SPI register handler */
                              &m_msndDataConfig,        /* SPI configuration register handler */
                              totalBytesToTransmit,     /* Total byte(s) to transfert to ND */
                              (uint16_t *)&m_spiTxBuff, /* Tx data */
                              (uint16_t *)&m_spiRxBuff);/* Rx data */
    
    // Set CS high
    gioSetBit(spiPORT3, CS_SPIPORT3_BIT_NEUTRON_DETECT, 1);

    // Copy data from buffer to data to return
    for(i=0; i< sizeof(m_spiRxBuff); i++){
        *data = m_spiRxBuff[i];
        data++;
    }

    return NeutronDetector::ND_NO_ERROR;
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void NeutronDetectorComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // TODO
  }

  // ----------------------------------------------------------------------
  // Command handler implementations
  // ----------------------------------------------------------------------

  void NeutronDetectorComponentImpl ::
    ND_CollectData_cmdHandler(
        const FwOpcodeType opCode,
        const U32 cmdSeq
    )
  {
    // TODO
    this->cmdResponse_out(opCode,cmdSeq,Fw::COMMAND_OK);
  }

} // end namespace CubeRover
