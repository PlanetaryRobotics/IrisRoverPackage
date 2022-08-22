/**
 * @file NetworkInterface.hpp
 * @author Connor Colombo (colombo@cmu.edu)
 * @brief Enumeration of all special Telemetry (TM) and Telecommands (TC)
 * exchanged Directly between Ground and the network layer of Hercules
 * (bypassing any FPrime) bytes sequences.
 *
 * These exist only to allow hot diagnostics and fixes in case something breaks
 * in the stack making it difficult or impossible to get data in or out of the
 * normal FPrime pathway.
 *
 * @date 2022-08-22
 */

#ifndef CUBEROVER_WF121_WF121_GROUND_DIRECT_TMTC_HPP_
#define CUBEROVER_WF121_WF121_GROUND_DIRECT_TMTC_HPP_

#include <Fw/Types/BasicTypes.hpp>

namespace Wf121
{
    // Message sent from Hercules when the first available downlink opportunity
    // opens:
    static const uint8_t HELLO_EARTH_MESSAGE[] = "Hello Earth, this is Hercules on the Moon!";

    // Special (emergency) message Hercules downlinks if it had to drop
    // Uplinked data because its `m_xUdpRxPayloadQueue` buffer is full. This
    // likely indicates very bad overall system health and means important
    // commands won't get through quickly if at all. To remedy this, Ground can
    // send an emergency `GND_DIRECT_CMD_RESET_ALL_BUFFERS`.
    // * This is in plain ASCII so it's immediately readable from a packet
    //   viewer like YAMCS (even if slightly corrupted during the downlink process).
    const uint8_t CRITICAL_MSG__HERC_RX_BUFFER_FULL[] = "HER:CRITICAL--DROPPED-RX--BUFFER-FULL";

    // Special message sent from Ground telling Hercules to dump (throw away
    // and reset) all uplink and downlink buffers (`m_xUdpRxPayloadQueue` and
    // `m_xUdpTxPayloadQueue`).
    //
    // Intended Use Case:
    // Ground can tell Hercules is getting laggy because its uplink packet
    // responses are delayed, meaning it's either taking it a while to process
    // uplinked commands or to downlink telemetry. One solution is to just ask
    // WD to reset Hercules. If Ground doesn't want to do that and it's about
    // to send a *VERY* important command that Hercules needs to get ASAP,
    // Ground can first send a UDP packet containing just this as the payload
    // to tell Hercules to flush all the buffers.
    // * This is intentionally a long string of bytes so it's very unlikely a normal message gets turned into it.
    // * This is intentionally an ascii string so it can be read easily in comm logs (PCAPs).
    //      * A final small benefit of being ASCII is that, in dire circumstances, this is something that someone could type directly into a YAMCS terminal.
    //
    // NOTE: This bypasses all FPrime uplink data handlers. Normally this
    // wouldn't be a great idea. We're only allowing it in this special case
    // since, if we have to use this command, Ground has determined it's not
    // possible to get data to the FPrime stack in a timely manner.
    const uint8_t GND_DIRECT_CMD_RESET_ALL_BUFFERS[] = "RESET-ALL-COMMS-BUFFERS-NOW";
    // Fixed acknowledgement string that the above command was received:
    // last byte should be replaced with the number of resets done (i.e. let's us know if any of the Queues *couldn't* be reset because they're broken)
    // * This is in plain ASCII so it's immediately readable from a packet viewer like YAMCS.
    const uint8_t GND_DIRECT_RSP_RESET_ALL_BUFFERS[] = "RESET-ALL-COMMS-BUFFERS-DONE:X";

    // Struct used for acknowledging that an uplinked UDP packet was
    // successfully received from the Radio (sent back to the Radio to be
    // downlinked). Useful for determining if there's an issue in the
    // networking stack, like very filled / poorly serviced buffers).
    //
    // Format:
    //      - "HER:" // Fixed 4B header (parallel of "RAD:")
    //      - [time (in ms since Hercules boot) that the rover generated this response - i.e. when it finished processing the command] // (4B, little endian)
    //      - [length of UDP payload received] // (2B, little endian)
    union HerculesRadioUplinkResponse
    {
        uint8_t rawData[sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t)];
        struct
        {
            uint32_t fixedHeader = 0x3A'52'45'48; // 'HER:' (in little-endian)
            uint32_t roverResponseGenerationTimeMs = 0;
            uint16_t payloadBytesReceived; // Number of bytes in the UDP payload
        } __attribute__((packed, aligned(1)));

        HerculesRadioUplinkResponse(uint16_t pbr) : payloadBytesReceived(pbr)
        {
            // Set the time the rover created this uplink response:
            roverResponseGenerationTimeMs = static_cast<uint32_t>(getTime().get_time_ms());
        };
    };
}

#endif /* CUBEROVER_WF121_WF121_GROUND_DIRECT_TMTC_HPP_ */
