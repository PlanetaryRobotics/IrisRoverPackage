/**
 * @file UdpPayload.hpp
 * @author Connor Colombo (colombo@cmu.edu)
 * @brief Simple container class for UDP Payloads (not the datagrams
 * themselves, just the payload data).
 *
 * @date 2022-08-17
 */

#ifndef CUBEROVER_WF121_WF121_UDP_PAYLOAD_HPP_
#define CUBEROVER_WF121_WF121_UDP_PAYLOAD_HPP_

#include "FreeRTOS.h"
#include <Include/FswPacket.hpp>

#include <cstring>

namespace Wf121
{
    // Simple container struct for UDP payload data.
    // (no mutex protection here b/c this goes inside a FreeRTOS IPC Queue).
    //
    // For Reads:
    // The idea is that every time a valid UDP payload is received from
    // the Radio via a BGAPI event, we pass one of these into the
    // `xUdpRxPayloadQueue`. Uses a fixed size buffer since FreeRTOS Queues
    // only pass fixed size data (and it's risky to pass pointers through
    // it if they might go out of scope).
    //
    // For Writes:
    // The reverse is true for writes, every time a Task wants to send data
    // to the Radio's UDP endpoint, it should push it into
    // `xUdpTxPayloadQueue`, which gets drained by `Wf121UdpTxTask`.
    struct UdpPayload
    {
        uint16_t dataSize;
        uint8_t data[WF121_UDP_MAX_PAYLOAD] __attribute__((aligned(8))); // align the start of the array for more optimizable copies

        UdpPayload() : dataSize(0)
        {
            /* nothing else to do here */
        }

        // Clear all data (overwrite with fixed sentinel byte):
        // (NOTE: don't want to just blindly do this on every ctor call
        // since, for a buffer this size, it's slightly expensive)
        void clear()
        {
            dataSize = 0;
            // Write 0xFA to all "empty" bytes in this array, this just makes debugging easier
            memset(this->data, 0xAB, WF121_UDP_MAX_PAYLOAD);
        }
    };

    struct UdpRxPayload : public UdpPayload
    {
        // At the moment, doesn't require any extra special data.
    };

    struct UdpTxPayload : public UdpPayload
    {
        // At the moment, doesn't require any extra special data.
    };
}

#endif /* CUBEROVER_WF121_WF121_UDP_PAYLOAD_HPP_ */