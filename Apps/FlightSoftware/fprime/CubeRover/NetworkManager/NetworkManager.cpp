// ======================================================================
// \title  NetworkManagerComponentImpl.cpp
// \author justin
// \brief  cpp file for NetworkManager component implementation class
//
// \copyright
// Copyright 2009-2015, by the California Institute of Technology.
// ALL RIGHTS RESERVED.  United States Government Sponsorship
// acknowledged.
//
// ======================================================================


#include <CubeRover/NetworkManager/NetworkManager.hpp>
#include "Fw/Types/BasicTypes.hpp"

namespace CubeRover {

  // ----------------------------------------------------------------------
  // Construction, initialization, and destruction
  // ----------------------------------------------------------------------

  NetworkManagerComponentImpl ::
#if FW_OBJECT_NAMES == 1
    NetworkManagerComponentImpl(
        const char *const compName
    ) :
      NetworkManagerComponentBase(compName)
#else
    NetworkManagerComponentImpl(void)
#endif
  {

  }

  void NetworkManagerComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    NetworkManagerComponentBase::init(instance);
    unsigned no_transition_count = 0
    CubeRoverNetworkManager::CubeRoverNetworkStateMachine current_state = crnm.GetState();
    while(1){
        Wf121::ErrorCode ret = crnm.UpdateNetworkManager();
        CubeRoverNetworkManager::CubeRoverNetworkStateMachine new_state = crnm.GetState();
        
        if (current_state == UDP_CONNECTED)     // Connection & transport layer established
            break;
        else if (current_state == new_state)
            no_transition_count++;
        else
            log_ACTIVITY_HI_state_change(current_state, new_state);
        current_state = new_state;

        if (ret) {} // Need to handle error
        if (no_transition_count > MAX_FSM_NO_TRANSITION_COUNT) {
            // Need to handle error
            break;
        }
  }

  NetworkManagerComponentImpl ::
    ~NetworkManagerComponentImpl(void)
  {
    // XXX: Shutdown wifi?
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void NetworkManagerComponentImpl ::
    fileUplinkBufferGet_handler(
        const NATIVE_INT_TYPE portNum,
        U32 size
    )
  {
    // TODO
  }

  void NetworkManagerComponentImpl ::
    fileDownlinkBufferSendIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    // TODO
  }

  void NetworkManagerComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
    // Read and process input/output queues
    CubeRoverNetworkManager::CubeRoverNetworkStateMachine current_state = crnm.GetState();
    int8_t rssi = crnm.GetSignalRssi();
    tlmWrite_RSSI(rssi)
    int8_t snr = crnm.GetSignalNoiseRatio();
    CubeRoverNetworkManager::CubeRoverSignalLevels signal_level = crnm.GetSignalLevel();    // Probably won't use this
  }

} // end namespace CubeRover
