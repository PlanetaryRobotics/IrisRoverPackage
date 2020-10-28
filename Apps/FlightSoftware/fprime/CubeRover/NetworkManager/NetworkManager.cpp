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
#include <cstring>

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
    current_state = CubeRoverNetworkManager::UNINITIALIZED;
  }

  void NetworkManagerComponentImpl ::
    init(
        const NATIVE_INT_TYPE instance
    )
  {
    NetworkManagerComponentBase::init(instance);
    unsigned no_transition_count = 0;
    current_state = crnm.GetState();
    while (1) {
        Wf121::ErrorCode ret = crnm.UpdateNetworkManager();
        CubeRoverNetworkManager::CubeRoverNetworkStateMachine new_state = crnm.GetState();
        
        if (current_state == CubeRoverNetworkManager::UDP_CONNECTED)     // Connection & transport layer established
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
  }

  NetworkManagerComponentImpl ::
    ~NetworkManagerComponentImpl(void)
  {
    // XXX: Shutdown wifi?
  }

  // ----------------------------------------------------------------------
  // Handler implementations for user-defined typed input ports
  // ----------------------------------------------------------------------

  Fw::Buffer NetworkManagerComponentImpl ::
    fileUplinkBufferGet_handler(
        const NATIVE_INT_TYPE portNum,
        U32 size
    )
  {
    // Okay 3 different size variables:
    // - arg U32 size: Hold over from this type for BufferManager supposedly requested buffer size
    // - MAX_SIZE_PAYLOAD 4096: Maximum expected UDP payload size supposedly...
    // - payloadSize: Actual size of the payload in UDP header
    // Since our buffer is statically allocated we are going to ignore arg
    // XXX: The following is a non-thread safe impl (i.e. poop shared pointers) using a single payload statically allocated buffer
    // for transferring uplink commands to GroundInterface. This is okay if GroundInterface discards
    // the pointer immediately before this function is called again.
    FW_ASSERT(size <= MAX_SIZE_PAYLOAD);
    Fw::Buffer buffer;
    U16 bytesRead;
    U16 headerSize = 8;
    U32 payloadSize;
    U64 data_ptr = reinterpret_cast<U64>(fileUplinkBuffer);

    memset(fileUplinkBuffer, 0, MAX_SIZE_PAYLOAD);  // Clear one datagram buffer
    // XXX: Or maybe we shoould parse datagram header here w/ checksum and only pass payload
    // UDPReceiver!?

    crnm.ReceiveUdpData(fileUplinkBuffer, headerSize, &bytesRead, CubeRoverNetworkManager::UdpReadMode::WAIT_UNTIL_READY | CubeRoverNetworkManager::UdpReadMode::PEEK_READ, 10);     // Read UDP header to get payload size

    if (bytesRead == headerSize) {  // check how big the packet actually is, then consume the bytes from the ring buffer
        memcpy(&payloadSize, fileUplinkBuffer+4, sizeof(payloadSize));  // byte 4 of UDP header
        crnm.ReceiveUdpData(fileUplinkBuffer, payloadSize, &bytesRead, CubeRoverNetworkManager::UdpReadMode::WAIT_UNTIL_READY | CubeRoverNetworkManager::UdpReadMode::NORMAL_READ, 10);

        if (bytesRead != payloadSize) {} // FIXME: Error, bytes read didnt match payload size
    }

    buffer.set(this->getInstance(), 0, data_ptr, payloadSize);    // Only one buffer->buferID = 0
    return buffer;
  }

  void NetworkManagerComponentImpl ::
    fileDownlinkBufferSendIn_handler(
        const NATIVE_INT_TYPE portNum,
        Fw::Buffer &fwBuffer
    )
  {
    U8 *buffer = reinterpret_cast<U8 *>(buffer.getdata());
    U32 payloadSize = buffer.getsize();
    crnm.SendUdpData(buffer, payloadSize, 1000);   // FIXME: What is an appropriate timeout 1s check units
    schedIn_handler(0, 0);
  }

    void NetworkManagerComponentImpl::update() {
        Wf121::Error errorCode = crnm.ExecuteCallbacks();   // TODO: Check error

        CubeRoverNetworkManager::CubeRoverNetworkStateMachine updated_state = crnm.GetState();
        if (updated_state != current_state) {
            log_ACTIVITY_HI_state_change(current_state, new_state);
            // TODO: TRIGGER MODEMANAGER ON LOS
        }
        tlmWrite_RSSI(crnm.GetSignalRssi());
        tlmWrite_SNR(crnm.GetSignalNoiseRatio());
        tlmWrite_pkt_recv(crnm.GetNbOfBytesReceived);
        tlmWrite_pkt_sent(crnm.GetNbOfBytesSent);
    }

  void NetworkManagerComponentImpl ::
    schedIn_handler(
        const NATIVE_INT_TYPE portNum,
        NATIVE_UINT_TYPE context
    )
  {
      update();
  }

} // end namespace CubeRover
