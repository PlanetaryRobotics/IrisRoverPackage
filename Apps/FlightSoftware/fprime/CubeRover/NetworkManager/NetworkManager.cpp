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
#include <CubeRover/GroundInterface/GroundInterface.hpp>
#include <CubeRover/WatchDogInterface/WatchDogInterface.hpp>
#include "CubeRover/Wf121/Wf121.hpp"
#include "Fw/Types/BasicTypes.hpp"
#include <cstring>

extern CubeRover::WatchDogInterfaceComponentImpl watchDogInterface;

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
    m_current_state = CubeRoverNetworkManager::UNINITIALIZED;
    wired_wifi_reset_cnt = 0;
  }

  void NetworkManagerComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    NetworkManagerComponentBase::init(instance);
    
    telem_send_limit_cnt = 0;
    stuck_state_wifi_reset_cnt = 0;
    unsigned no_transition_count = 0;
    m_current_state = m_crnm.GetState();
    bool success = false;
    while (!success) {
        Wf121::ErrorCode errorCode;
        while (no_transition_count < MAX_FSM_NO_TRANSITION_COUNT) {
            errorCode = m_crnm.UpdateNetworkManager();

            if (errorCode == Wf121::TRY_AGAIN) {
                no_transition_count++;
                continue;
            } else if (errorCode != Wf121::NO_ERROR) {
                log_WARNING_HI_ExecuteCallbackError(errorCode);
                no_transition_count++;
                continue;
            }
            CubeRoverNetworkManager::CubeRoverNetworkStateMachine new_state = m_crnm.GetState();
            
            if (m_current_state == new_state)
                no_transition_count++;
            else
            {
                log_ACTIVITY_HI_StateChange(m_current_state, new_state);
                // Update Telemetry with error code
                tlmWrite_WIFIErrorStatus(static_cast<U16>(errorCode));
                // Update Telemetry with new state
                tlmWrite_WIFIStateStatus(static_cast<WIFIState>(static_cast<U8>(new_state)));
            }
            
            m_current_state = new_state;
            
            if (m_current_state == CubeRoverNetworkManager::UDP_CONNECTED) {     // Connection & transport layer established
                success = true;
                break;
            }
        }
        // Update Telemetry with error code
        tlmWrite_WIFIErrorStatus(static_cast<U16>(errorCode));
        // Check if we're in persistent state WATCHDOG (m_persistent_state == 0) and we have tried too many times
        if (m_persistent_state == 0 && no_transition_count >= MAX_FSM_NO_TRANSITION_COUNT) {
            log_FATAL_WF121InitializationFailed();
            // Check if we have reset the wifi more than wired_wifi_reset_cnt_max times. If so, we stop initializing wifi and move on
            if(wired_wifi_reset_cnt >= wired_wifi_reset_cnt_max)
                break;
            // TODO: Notify ground that we have skipped initializing wifi as we have reset a set amount of times
            watchDogInterface.Reset_Specific_Handler(4 /*Reset_Radio = 4*/);
            m_crnm.ResetState();
            wired_wifi_reset_cnt++;
            no_transition_count = 0;
        }
        // Check if we're in persistent state WIFI (m_persistent_state == 1) and we have tried too many times
        else if (m_persistent_state == 1 && no_transition_count >= MAX_FSM_NO_TRANSITION_COUNT) {
            log_FATAL_WF121InitializationFailed();
            watchDogInterface.Reset_Specific_Handler(4 /*Reset_Radio = 4*/);
            m_crnm.ResetState();
            no_transition_count = 0;
            // This reset of wifi will happen forever until we connect
        }
    }
  }

  NetworkManagerComponentImpl ::
    ~NetworkManagerComponentImpl(void)
  {
    // This should NEVER get called
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  void NetworkManagerComponentImpl ::
    downlink_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    uint8_t *buffer = reinterpret_cast<uint8_t *>(fwBuffer.getdata());
    uint32_t payloadSize = fwBuffer.getsize();
    Wf121::ErrorCode e = m_crnm.SendUdpData(buffer, payloadSize, 100000);   // FIXME: What is an appropriate timeout 1s check units
    update();
  }

  void NetworkManagerComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
      update();
      getUplinkDatagram();
  }

    void NetworkManagerComponentImpl::update() {
        Wf121::ErrorCode errorCode = m_crnm.UpdateNetworkManager();   // TODO: Check error



        CubeRoverNetworkManager::CubeRoverNetworkStateMachine updated_state = m_crnm.GetState();
        if (updated_state != m_current_state) {
            log_ACTIVITY_HI_StateChange(m_current_state, updated_state);
            tlmWrite_WIFIStateStatus(static_cast<WIFIState>(static_cast<U8>(updated_state)));
            // TODO: TRIGGER MODEMANAGER ON LOS (check errorCode specifically)
        }
        m_current_state = updated_state;

        // Check if we have been in stuck in a state state for over MAX_FSM_NO_TRANSITION_COUNT and reset wifi chip if so
        if(m_current_state != CubeRoverNetworkManager::CubeRoverNetworkStateMachine::UDP_CONNECTED)
        {
            stuck_state_wifi_reset_cnt++;
            if(stuck_state_wifi_reset_cnt >= MAX_FSM_NO_TRANSITION_COUNT)
            {
                watchDogInterface.Reset_Specific_Handler(4 /*Reset_Radio = 4*/);
                m_crnm.ResetState();
                stuck_state_wifi_reset_cnt = 0;
            }
        }

        // TODO: UPDATE THESE ONCE PER TELEM DOWNLINK. DOING IT ON THE RATE GROUP IS TOO OFTEN
        if(telem_send_limit_cnt >= telem_send_limit_cnt_max)
        {
            tlmWrite_RSSI(m_crnm.GetSignalRssi());
            tlmWrite_SNR(m_crnm.GetSignalNoiseRatio());
            tlmWrite_PktRecv(m_crnm.GetNbOfBytesReceived());
            tlmWrite_PktSent(m_crnm.GetNbOfBytesSent());
            // Update Telemetry with error code
            tlmWrite_WIFIErrorStatus(static_cast<U16>(errorCode));
        }

    }
  
    void NetworkManagerComponentImpl::getUplinkDatagram() {
        U16 bytesRead;
        Wf121::DataSize16 payloadSize;

        memset(m_fileUplinkBuffer, 0, WF121_UDP_MAX_PAYLOAD);

        m_crnm.ReceiveUdpData(reinterpret_cast<uint8_t *>(&payloadSize), sizeof(Wf121::DataSize16), &bytesRead, CubeRoverNetworkManager::UdpReadMode::PEEK_READ, 10);        // Read size of UDP payload
        if (bytesRead == 0)
            return;     // No data to read

        if (bytesRead == sizeof(Wf121::DataSize16)) {
            m_crnm.ReceiveUdpData(reinterpret_cast<uint8_t *>(&payloadSize), sizeof(Wf121::DataSize16), &bytesRead, CubeRoverNetworkManager::UdpReadMode::NORMAL_READ, 10);  // Flush the size, we read it in already
            m_crnm.ReceiveUdpData(m_fileUplinkBuffer, payloadSize, &bytesRead, CubeRoverNetworkManager::UdpReadMode::NORMAL_READ, 10);

            if (bytesRead != payloadSize) {} // FIXME: Error, bytes read didnt match payload size
        }

        Fw::Buffer buffer(0, 0, reinterpret_cast<U64>(m_fileUplinkBuffer), payloadSize);
        uplink_out(0, buffer);
        update();
    }


} // end namespace CubeRover
