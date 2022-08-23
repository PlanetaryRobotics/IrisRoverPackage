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
        uint8_t data[WF121_UDP_MAX_PAYLOAD] __attribute__((aligned(8))); // align to uint64_t boundary since all FPrime buffers get handled as U64 (for some reason?)

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

        // Copies data from the given buffer into this payload (capping it at
        // the max size):
        void copyIn(size_t bufferLen, uint8_t *buffer)
        {
            uint16_t bytesToCopy = bufferLen <= WF121_UDP_MAX_PAYLOAD ? bufferLen : WF121_UDP_MAX_PAYLOAD;
            memcpy(this->data, buffer, bytesToCopy);
        }
    };

    // Create aliases for Payloads going in each direction:
    // If this changes (i.e. UdpRxPayload or UdpTxPayload become a subclass of
    // UdpPayload that contain extra fields), check the codebase for any
    // static_cast between the different UdpPayload types and make sure they
    // won't become broken (there is at least one at the time of writing).
    // ^ compiler should catch UdpRxPayload<->UdpTxPayload casts that break in
    // this way too since they'll be siblings and not directly related through
    // inheritance.
    typedef UdpPayload UdpRxPayload;
    typedef UdpPayload UdpTxPayload;
}

#endif /* CUBEROVER_WF121_WF121_UDP_PAYLOAD_HPP_ */