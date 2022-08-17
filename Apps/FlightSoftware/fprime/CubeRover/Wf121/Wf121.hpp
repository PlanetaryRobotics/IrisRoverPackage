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