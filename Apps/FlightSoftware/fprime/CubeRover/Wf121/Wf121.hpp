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

#include <CubeRover/Wf121/NetworkInterface.hpp>
#include <CubeRover/Wf121/Wf121SerialInterface.hpp>
#include <CubeRover/Wf121/Wf121Parser.hpp>
#include <CubeRover/Wf121/Wf121RxTask.hpp>

// ! TODO: (WORKING-HERE): [CWC] Process callback and dispatch to appropriate NetworkInterface functions. Then:
// - Finish DirectMessageDriver.
// - Update state inside network interface
// - Create callback hooks for BGAPI CBs we care about (incl. Signal Quality)
// - Create Queue to pipe datagrams from callbacks to network manager (or whoever else).
//    https://www.freertos.org/Inter-Task-Communication.html
//    https://www.digikey.com/en/maker/projects/introduction-to-rtos-solution-to-part-5-freertos-queue-example/72d2b361f7b94e0691d947c7c29a03c9
// - Reset radio if no heartbeat for some multiple of expected time?
//
// - Lastly go back and review diffs from checkpoint commit (specifically notifications.c)
//    - Make sure those changes match with the current DMA-ISR system.
// ^ Copy this over to _wifi_todo.md

namespace Wf121
{
  // These three parameters control the setup of the task that handles data received from the WF121 Radio:
  static const NATIVE_INT_TYPE WF121_RX_TASK_PRIORITY = 15; // Slightly less than WatchDogInterface
  static const NATIVE_INT_TYPE WF121_RX_TASK_STACK_SIZE = 512;
  static const NATIVE_INT_TYPE WF121_RX_TASK_CPU_AFFINITY = -1;

  class RadioDriver : public virtual Wf121RxCallbackProcessor
  {

    // Interface responsible for processing data from and sending data to the
    // radio:
    NetworkInterface m_networkInterface;

    // FreeRTOS Task responsible for handling incoming data from the
    Wf121RxTask m_serialRxTask;

    /**
     * @brief The callback invoked by the `Wf121RxTask` when it has received a message.
     *
     * @param msg The parsed message received from the WF121 Radio.
     */
    virtual void rxCallback(Wf121Parser::GenericMessage &msg);

    // Begin all processes (once outside code is ready):
    void init();

    // Constructor (just set up data structures):
    RadioDriver();

    // Destructor:
    ~RadioDriver();
  };
}

#endif /* CUBEROVER_WF121_WF121_CORE_HPP_ */