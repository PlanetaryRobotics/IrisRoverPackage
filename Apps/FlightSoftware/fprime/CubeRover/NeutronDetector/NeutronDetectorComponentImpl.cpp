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

  // Global variable required to process etpwm interrupt
  NeutronDetector::MuxPortMap g_muxReadInput;

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
    for(i=0; i<TOTAL_MSND_PLATE; i++){
      gioSetBit(m_muxPlateSelect[i].port, m_muxPlateSelect[i].bit, 0);
    }

    for(i=0; i<TOTAL_MUX_SENSOR_SELECT; i++){
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

      g_muxReadInput.port = gioPORTA;
      g_muxReadInput.bit = 1;
  }


  /**
   * @brief      Setup the detector
   *
   * @return     The neutron detector error
   */
  NeutronDetector::Error NeutronDetectorComponentImpl ::setupDetector(){
    // map outputs
    mapMuxOutputs();

    // PWM timer is used to set clock signal
    // Set through HALCOGEN
    // HALCOGEN must be set to:
    // 4000ns period (250KHz)
    // Up-count mode (by default)
    // CMPA set to 50% duty cycle, it generates the clock signal output to the sensor
    // CMPB set 97% duty cycle, it triggers the read of the data
    // Interrupt is set to trigger on event set to CTR_UP_CMPB
    // Enable PWM6 interrupt in VIM in Halcogen
    etpwmInit();

    // Clear event interrupt flag
    etpwmClearEventFlag(TIMER_EPWM_REG, Event_Interrupt);

    // Start timer B clock
    etpwmStartTBCLK();

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

static uint8_t g_bitToRead = 0;
static uint8_t *g_msndByte = NULL;
bool g_readCompleted;

extern "C"{
  void etpwmNotification(etpwmBASE_t *node)
  {
      if(g_bitToRead > 0){
         *g_msndByte |= gioGetBit(g_muxReadInput.port, g_muxReadInput.bit) << (g_bitToRead - 1);
          g_bitToRead--;
      }
      else{
          g_readCompleted = true; // no more bit to read
      }
  }
}

  NeutronDetector::Error NeutronDetectorComponentImpl ::  spiTransmitAndReceiveData(uint16_t totalBytesToTransmit,
                                                                                    uint16_t * txBuff,
                                                                                    uint16_t * rxBuff){
      uint8_t i;

      g_msndByte = (uint8_t *)rxBuff;

      for(i=0 ; i<totalBytesToTransmit; i++){
          g_bitToRead = 8; // 8 bits per byte
          g_readCompleted = false;

          // Enable capture compare. Set event interrupt flag when compare B is triggered
          // on first event.
          etpwmEnableInterrupt(TIMER_EPWM_REG, CTR_UP_CMPB, EventPeriod_FirstEvent);

          // block until all bits have been pushed
          while(g_readCompleted == false);

          // increment pointer
          g_msndByte++;
      }

      // Stop the timer interrupt
      etpwmDisableInterrupt(TIMER_EPWM_REG);

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
    gioSetBit(gioPORTA, CS_SPIPORT3_BIT_NEUTRON_DETECT, 0);

    spiTransmitAndReceiveData(totalBytesToTransmit,     /* Total byte(s) to transfert to ND */
                              (uint16_t *)&m_spiTxBuff, /* Tx data */
                              (uint16_t *)&m_spiRxBuff);/* Rx data */
    
    // Set CS high
    gioSetBit(gioPORTA, CS_SPIPORT3_BIT_NEUTRON_DETECT, 1);

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
      // test
      NeutronDetector::NeutronSensorData test[TOTAL_MSND_PLATE*TOTAL_MSND_PER_PLATE];

      getSensorArray((NeutronDetector::NeutronSensorArray)test);
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
