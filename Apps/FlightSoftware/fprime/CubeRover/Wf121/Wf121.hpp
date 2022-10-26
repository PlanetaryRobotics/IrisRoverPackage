/**
 * @file Wf121Core.hpp
 * @author Connor Colombo (colombo@cmu.edu)
 * @brief Core for handling communications with the custom radio firmware
 *  running on the BlueGiga WF121 Radio.
 *
 * This file defines `RadioDriver` which should be the main interface point
 * between external code and the WF121 Radio.
 *
 * @date 2022-08-13
 */

#ifndef CUBEROVER_WF121_WF121_CORE_HPP_
#define CUBEROVER_WF121_WF121_CORE_HPP_

#include "FreeRTOS.h"
#include "os_portmacro.h"
#include "os_semphr.h"
#include "os_task.h"
#include <Os/Mutex.hpp>

#include <CubeRover/Wf121/NetworkInterface.hpp>
#include <CubeRover/Wf121/Wf121SerialInterface.hpp>
#include <CubeRover/Wf121/Wf121Parser.hpp>
#include <CubeRover/Wf121/Wf121RxTask.hpp>
#include <CubeRover/Wf121/Wf121UdpTxTask.hpp>

namespace Wf121
{
  // These three parameters control the setup of the task that handles data received from the WF121 Radio:
  static const NATIVE_INT_TYPE WF121_RX_TASK_PRIORITY = 15; // Slightly less than WatchDogInterface
                                                            // NOTE: Stack size is in words. Make sure there's enough room for the overhead (min task size) plus some overhead. Use `uxTaskGetStackHighWaterMark(NULL)` to tune.
  static const NATIVE_INT_TYPE WF121_RX_TASK_STACK_SIZE = configMINIMAL_STACK_SIZE + 512;
  static const NATIVE_INT_TYPE WF121_RX_TASK_CPU_AFFINITY = -1;

  // These three parameters control the setup of the task that handles sending UDP data to the WF121 Radio:
  static const NATIVE_INT_TYPE WF121_UDP_TX_TASK_PRIORITY = WF121_RX_TASK_PRIORITY - 1; // Slightly less than WF121_RX_TASK_PRIORITY (since we want to be interrupted by any callback data that might be coming in )
  // NOTE: Stack size is in words. Make sure there's enough room for the overhead (min task size) plus some overhead. Use `uxTaskGetStackHighWaterMark(NULL)` to tune.
  static const NATIVE_INT_TYPE WF121_UDP_TX_TASK_STACK_SIZE = configMINIMAL_STACK_SIZE + 256; // Doesn't handle BGAPI callback processing so it can be much shallower than the RX task.
  static const NATIVE_INT_TYPE WF121_UDP_TX_TASK_CPU_AFFINITY = -1;

  // Use an enum of specific values - not just a bool - so memory corruption /
  // fading (if in SRAM) can be detected and corrected by resetting to a default:
  enum BgApiPassThroughState
  {
    BGAPI_PASSTHROUGH_ENABLED = 0x9A550060,
    BGAPI_PASSTHROUGH_DISABLED = 0x0FF09A55
  };
  // Whether BGAPI pass through mode is enabled. Persistent. Default: false (normal Hercules-Radio communications).
  extern BgApiPassThroughState persistent_bgapi_passthrough;
  Os::Mutex persistent_bgapi_passthrough_mutex;
  static BgApiPassThroughState BGAPI_PASSTHROUGH_DEFAULT = BGAPI_PASSTHROUGH_DISABLED;

  // Getter that checks if the value is valid and corrects if not:
  // (accounts for possible memory fading if stored in SRAM and a POR occurred)
  BgApiPassThroughState getPersistentBgApiPassthrough()
  {
    BgApiPassThroughState retVal;
    persistent_bgapi_passthrough_mutex.lock();
    // Check the passthrough state (make sure it's valid):
    switch (persistent_bgapi_passthrough)
    {
    case BGAPI_PASSTHROUGH_ENABLED:
    case BGAPI_PASSTHROUGH_DISABLED:
      // All fine. Do nothing.
      asm("  NOP");
      break;
    default:
      // Value not recognized. Go to default:
      persistent_bgapi_passthrough = BGAPI_PASSTHROUGH_DEFAULT;
    }
    retVal = persistent_bgapi_passthrough;
    persistent_bgapi_passthrough_mutex.unLock();
    return retVal;
  }

  // Convenient shorthand:
  inline bool persistentBgApiPassthroughEnabled()
  {
    return getPersistentBgApiPassthrough() == BgApiPassThroughState::BGAPI_PASSTHROUGH_ENABLED;
  }

  // Changes the persistent_bgapi_passthrough to the given boolean
  // enabled state, performing any necessary supporting operations
  // depending on the target state.
  // Returns whether a change was made.
  bool changeBgApiPassthroughState(bool enabled);

  class RadioDriver : public virtual Wf121RxCallbackProcessor
  {
  public:
    // Interface responsible for processing data from and sending data to the
    // radio:
    NetworkInterface m_networkInterface;

    // Begin all processes (once outside code is ready):
    void init();

    // Constructor (just set up data structures):
    RadioDriver();

    // Destructor:
    ~RadioDriver();

  private:
    // FreeRTOS Task responsible for handling incoming data from the Radio
    // over UART:
    Wf121RxTask m_serialRxTask;

    // FreeRTOS Task responsible for handling sending payload data to the
    // Radio's UDP endpoint over UART:
    Wf121UdpTxTask m_serialUdpTxTask;

    /**
     * @brief The callback invoked by the `Wf121RxTask` when it has received a message.
     *
     * @param msg The parsed message received from the WF121 Radio.
     */
    virtual void rxCallback(Wf121Parser::GenericMessage &msg);
  };
}

#endif /* CUBEROVER_WF121_WF121_CORE_HPP_ */
