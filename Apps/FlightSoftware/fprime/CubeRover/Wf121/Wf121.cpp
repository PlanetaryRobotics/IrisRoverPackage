#include <CubeRover/Wf121/Wf121.hpp>

namespace Wf121
{
  RadioDriver::RadioDriver() : m_networkInterface(),
                               m_serialRxTask(),
                               m_serialUdpTxTask(&m_networkInterface)
  {
    // nothing else to do here.
  }

  RadioDriver::~RadioDriver()
  {
    // Do Not Use
  }

  void RadioDriver::init()
  {
    // Make sure NetworkInterface is ready to receive data:
    m_networkInterface.init();

    // Setup serial:
    Wf121Serial::init();

    // Initialize the RX Task:
    m_serialRxTask.registerCallback(this);
    ::Os::Task::TaskStatus rxTaskStat = m_serialRxTask.startTask(WF121_RX_TASK_PRIORITY,
                                                                 WF121_RX_TASK_STACK_SIZE,
                                                                 WF121_RX_TASK_CPU_AFFINITY);
    // Assert that this will always be started successfully. If it isn't, we're screwed.
    configASSERT(rxTaskStat == Os::Task::TASK_OK);

    // Initialize the UDP TX Task:
    ::Os::Task::TaskStatus txTaskStat = m_serialUdpTxTask.startTask(WF121_UDP_TX_TASK_PRIORITY,
                                                                    WF121_UDP_TX_TASK_STACK_SIZE,
                                                                    WF121_UDP_TX_TASK_CPU_AFFINITY);
    // Assert that this will always be started successfully. If it isn't, we're screwed.
    configASSERT(txTaskStat == Os::Task::TASK_OK);

    // Now that everything is ready to receive data from the radio,
    // tell the radio to send us data whenever it wants:
    Wf121Serial::ReadyForData();
  }

  void RadioDriver::rxCallback(Wf121Parser::GenericMessage &msg)
  {
    switch (msg.headerType)
    {
    case Wf121Parser::HeaderType::BGAPI:
      // Dispatch to BGAPI handler:
      this->m_networkInterface.processBgApiMessage(msg.bgApiHeader(), msg.payloadSize, msg.payloadBuffer);
      break;
    case Wf121Parser::HeaderType::DIRECT_MESSAGE:
      // Dispatch to DIRECT_MESSAGE:
      this->m_networkInterface.processDirectMessage(msg.payloadSize, msg.payloadBuffer);
      break;
    case Wf121Parser::HeaderType::INVALID:
      // Callback shouldn't have been triggered if the header was invalid.
      // Nothing we should do here per se, just weird.
      break;
    default:
      // How did we end up here? Doesn't match any enum? Looks like data
      // corruption. Halt to make WD reset us:
      configASSERT(pdFALSE);
    }
  }
}
