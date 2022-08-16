#include <CubeRover/Wf121/Wf121.hpp>

namespace Wf121
{
  RadioDriver::RadioDriver() : m_networkInterface(), m_serialRxTask()
  {
    // nothing else to do here.
  }

  RadioDriver::~RadioDriver()
  {
    // Do Not Use
  }

  RadioDriver::init()
  {
    // Make sure NetworkInterface is ready to receive data:
    m_networkInterface.Init();

    // Setup serial:
    Wf121Serial::init();

    // Initialize the Rx Task:
    m_serialRxTask.registerCallback(this);
    ::Os::Task::TaskStatus taskStat = m_rxTask.startTask(WF121_RX_TASK_PRIORITY,
                                                         WF121_RX_TASK_STACK_SIZE,
                                                         WF121_RX_TASK_CPU_AFFINITY);
    // Assert that this will always be started successfully. If it isn't, we're screwed.
    assert(taskStat == Os::Task::TASK_OK);

    // Now that everything is ready to receive data from the radio,
    // tell the radio to send us data whenever it wants:
    Wf121Serial::ReadyForData();
  }

  void RadioDriver::rxCallback(WatchDogMpsm::Message &msg, bool goodParity)
  {
    // If the message failed its parity check we don't even try to match it against any previously
    // sent messages. Instead, just warn about this occurring. We'll rely on timeout to eventually
    // clear up the tx command status structure for whatever tx command resulted in this
    // message with bad parity (if it was even a command to begin with)
    if (!goodParity)
    {
      this->log_WARNING_HI_WatchDogIncorrectResp(bad_parity);
    }

    Fw::Time now = getTime();

    // Uplink messages are different (specifically, they aren't a response to a Hercules command)
    // so we handle them separately
    if (msg.parsedHeader.lowerOpCode == UPLINK_OPCODE)
    {
      handleUplinkMsg(msg);
      return;
    }

    // Try to get the transmit status structure for the received message
    TxCommandStatus *cmdStatus = getTxCommandStatus(msg.parsedHeader.lowerOpCode);

    if (NULL == cmdStatus)
    {
      // TODO: Log error (which error?)
      debugPrintfToWatchdog("NULL cmdStatus\n");
      return;
    }

    // If we got a message, but the tx command status structure indicates that it is inactive
    // (i.e. that there was no transmitted message that is waiting for a response) then we want
    // to log an error, but continue on anyway so that we handle the message if it's telemetry.
    // In this case, since the tx command status structure is already inactive there is no need
    // to change it.
    //
    // On the other hand, if the status structure indicates that it *is* active but the sequence
    // number in the status structure is different than the sequence number of the received message,
    // then our handling depends on the ordering of the sequence numbers. If the received sequence
    // number is greater than the sequence number in the status structure (not sure how this could
    // happen), then we will reset the status structure and send a response for it, but we'll also
    // log an error about the incorrect sequence number. If the received sequence number is less
    // than the sequence number in the status structure, then presumably what happened is that
    // a previous command of the same type timed out while waiting for a response from WD, and
    // Hercules sent another command of the same type. Getting this response now, for the previous
    // message, means that for some reason WD was delayed in responding. In this case, we will not
    // clear the status structure, as the hope is that the WD will send another response for the
    // subsequent command that was sent. We'll still log an error in this case as well, but since
    // we don't know the correct sequence number to respond for the older tx message we will not
    // send a response. In either case (a sequence number mismatch of either ordering) we will
    // still handle the message (i.e. we'll process it if it is telemetry).
    //
    // In the nominal case, where everything in this response is what we would expect given the
    // contents of the tx status structure, we'll send a response for the tx command in question,
    // reset the tx status structure, and then handle the message (if it's telemetry).
    //
    // In all three of the above cases, any tx command structure manipulation will happen in the
    // section below in which the mutex is locked. However, any logging, responding to any
    // existing command, or handling of the message will be delayed until after the locked section.

    bool cmdInactive = false;
    bool rxOlderSeqNum = false;
    bool rxNewerSeqNum = false;
    FwOpcodeType txCmdOpCode = 0;
    U32 txCmdSeqNum = 0;
    bool txCmdSendResponse = false;
    uint32_t txTimeMillis = 0;

    { // The mutex must be unlocked before we exit this scope
      m_txCmdArray.cmdMutex.lock();

      if (!cmdStatus->active)
      {
        // Inactive tx command
        cmdInactive = true;
      }
      else
      {
        // Store the contents of the tx command structure prior to possible reset and so we can use
        // them outside of this block
        txCmdOpCode = cmdStatus->opcode;
        txCmdSeqNum = cmdStatus->seqNum;
        txCmdSendResponse = cmdStatus->sendResponse;
        txTimeMillis = cmdStatus->txTimeMillis;

        uint16_t ushortTxSeqNum = static_cast<uint16_t>(txCmdSeqNum);

        if (msg.parsedHeader.lowerSeqNum == ushortTxSeqNum)
        {
          // Matching sequence numbers. Just need to reset the tx status structure
          cmdStatus->reset();
        }
        else if (msg.parsedHeader.lowerSeqNum < ushortTxSeqNum)
        {
          // Received sequence number is less than (older than) the one in the tx status structure
          rxOlderSeqNum = true;
        }
        else
        {
          // Received sequence number is greater than (newer than) the one in the tx status structure
          rxNewerSeqNum = true;
          cmdStatus->reset();
        }
      }

      m_txCmdArray.cmdMutex.unLock();
    }

    if (cmdInactive)
    {
      // TODO: LOG ERROR (which error to log?)
      debugPrintfToWatchdog("cmdInactive: %d\n", msg.parsedHeader.lowerOpCode);
    }
    else if (rxOlderSeqNum)
    {
      // TODO: LOG ERROR (which error to log?)
      debugPrintfToWatchdog("rxOlderSeqNum: %d\n", msg.parsedHeader.lowerOpCode);
    }
    else if (rxNewerSeqNum)
    {
      // TODO: LOG ERROR (which error to log?)
      debugPrintfToWatchdog("rxNewerSeqNum: %d\n", msg.parsedHeader.lowerOpCode);

      // We want to respond to the old tx message. Make sure we don't try to send a response about any
      // of our fake opcodes, and don't send a response if we didn't want to send one when we sent the message
      if (txCmdOpCode != STROKE_OPCODE && txCmdOpCode != DOWNLINK_OPCODE && txCmdSendResponse)
      {
        this->cmdResponse_out(txCmdOpCode, txCmdSeqNum, Fw::COMMAND_EXECUTION_ERROR);
      }
    }
    else
    {
      if (txCmdOpCode == STROKE_OPCODE)
      {
        uint32_t nowMillis = static_cast<uint32_t>(now.get_time_ms());
        debugPrintfToWatchdog("Stroke response RTT: %u ms\n", nowMillis - txTimeMillis);
      }

      // We want to respond positively about the tx message. Make sure we don't try to send a response about any
      // of our fake opcodes, and don't send a response if we didn't want to send one when we sent the message
      if (txCmdOpCode != STROKE_OPCODE && txCmdOpCode != DOWNLINK_OPCODE && txCmdSendResponse)
      {
        this->cmdResponse_out(txCmdOpCode, txCmdSeqNum, Fw::COMMAND_OK);
      }
    }

    // Now we want to handle this message if it contains telemetry. For Reset_Specific and Downlink,
    // we're only looking for an ACK so we don't need to worry about doing anything with the responses for
    // those. However, the responses for the strokes will contain telemetry, and we do want to handle that.
    if (msg.parsedHeader.lowerOpCode == STROKE_OPCODE)
    {
      handleTelemetryMsg(msg);
    }

    return;
  }
}
