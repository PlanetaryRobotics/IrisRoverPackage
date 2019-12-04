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
  NeutronDetector::Error NeutronDetectorComponentImpl :: setMultiplexer(const NeutronDetector::SensorIterator sensor,
                                                                        const NeutronDetector::SensorPlateIterator sensorPlate){
    uint8_t i;
    // Plate selection is done by activating one output at a time
    // First disable all outputs
    resetIO();



    return NeutronDetector::ND_NO_ERROR;
  }

  /**
   * @brief      Setup the detector
   *
   * @return     The neutron detector error
   */
  NeutronDetector::Error NeutronDetectorComponentImpl ::setupDetector(){
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
    spiInit();

    g_muxReadInput.port = gioPORTA;
    g_muxReadInput.bit = 0;

    setupGioExpander();

    // Stop the timer interrupt
    etpwmDisableInterrupt(TIMER_EPWM_REG);

    // Start timer B clock
    etpwmStartTBCLK();

    return NeutronDetector::ND_NO_ERROR;
  }
  
  NeutronDetector::Error NeutronDetectorComponentImpl :: spiWriteRegister(const NeutronDetector::IoExpanderRegAddress addr, const uint8_t val){
      m_spiTxBuff[0] = 0x40; // address 7 MSB, write bit 8
      m_spiTxBuff[1] = addr;

      // set CS low
      gioSetBit(SPI_REG_PORT, SPI3_CS_BIT, 0);
      spiTransmitData(SPI_REG, &m_spiDataConfigHandler, sizeof(m_spiTxBuff), (uint16_t *)m_spiTxBuff);


      gioSetBit(SPI_REG_PORT, SPI3_CS_BIT, 1);

      return NeutronDetector::ND_NO_ERROR;
  }

  NeutronDetector::Error NeutronDetectorComponentImpl :: spiReadRegister(const NeutronDetector::IoExpanderRegAddress addr, uint8_t *val){
      m_spiTxBuff[0] = 0x41; // address 7 MSB, read bit 8
      m_spiTxBuff[1] = addr;
      return NeutronDetector::ND_NO_ERROR;
  }

  NeutronDetector::Error NeutronDetectorComponentImpl :: setupGioExpander(){
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

    NeutronDetector::SensorIterator sensor;
    NeutronDetector::SensorPlateIterator sensorPlate;

    // Scan through each sensor
    for(sensorPlate = 0; sensorPlate < TOTAL_MSND_PLATE; sensorPlate++){
     for(sensor = 0; sensor < TOTAL_MSND_PER_PLATE; sensor++){
        // Set the multiplexer to address the correct sensor
        setMultiplexer(sensor, sensorPlate);

        // Read the data from a single sensor and store it at the correct location
        readSensorData(array + sensor + sensorPlate*TOTAL_MSND_PER_PLATE);
     }
   }
    return NeutronDetector::ND_NO_ERROR;
  }

static uint8_t g_bitToRead;
static uint8_t *g_msndByte = NULL;
bool g_readCompleted;

// triggers on period event
extern "C"{
  void etpwmNotification(etpwmBASE_t *node)
  {
      // First read the bit
      if(g_bitToRead > 0){
         *g_msndByte |= gioGetBit(g_muxReadInput.port, g_muxReadInput.bit) << (g_bitToRead - 1);
          g_bitToRead--;

          // toggle output
          gioToggleBit(gioPORTA, 0);
      }
      else{
          g_readCompleted = true; // no more bit to read
          //no more toggling
          gioSetBit(gioPORTA, 0, 0);
          // Stop the timer interrupt
          etpwmDisableInterrupt(TIMER_EPWM_REG);
      }
  }
}

  void NeutronDetectorComponentImpl :: resetIO(){

  }

  /**
   * @brief      Read sensor data
   *
   * @param      data  The data
   *
   * @return     The Neutron sensor detector
   */
  NeutronDetector::Error NeutronDetectorComponentImpl ::
    readSensorData(NeutronDetector::NeutronSensorData *data){

    uint16_t totalBytesToTransmit = MSND_TX_PACKET_SIZE_BYTE;
    uint16_t i;

    g_msndByte = (uint8_t *)m_msndBuff;

    for(i=0 ; i<totalBytesToTransmit; i++){
        g_bitToRead = 8; // 8 bits per byte
        g_readCompleted = false;

        // Enable capture compare. Set event interrupt flag when compare B is triggered
        // on first event
        // Clear event interrupt flag
        etpwmClearEventFlag(TIMER_EPWM_REG, Event_Interrupt);
        etpwmEnableInterrupt(TIMER_EPWM_REG, CTR_PRD, EventPeriod_FirstEvent);

        // block until all bits have been pushed
        while(g_readCompleted == false);

        // increment pointer
        g_msndByte++;

        resetIO();
    }

    // Copy data from buffer to data to return
    for(i=0; i< sizeof(m_msndBuff); i++){
        *data = m_msndBuff[i];
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
      getSensorArray((NeutronDetector::NeutronSensorArray)m_neutronSensorArray);
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
