/*
 * ComPacket.hpp
 *
 *  Created on: May 24, 2014
 *      Author: Timothy Canham
 */

#ifndef COMPACKET_HPP_
#define COMPACKET_HPP_

#include <Fw/Types/Serializable.hpp>
#include <Include/FswPacket.hpp>    // PrimaryFlightController/FlightMCU

// Packet format:
// |32-bit packet type|packet type-specific data|

namespace Fw {

    class ComPacket: public Serializable {
        public:

            typedef enum {
                FW_PACKET_COMMAND = COMMAND_MAGIC, // !< Command packet type - incoming
                FW_PACKET_TELEM = TELEMETRY_MAGIC, // !< Telemetry packet type - outgoing
                FW_PACKET_LOG = LOG_MAGIC, // !< Log type - outgoing
                FW_PACKET_FILE = FILE_MAGIC, // !< File type - incoming and outgoing
                FW_PACKET_PACKETIZED_TLM = PACKETIZED_TLM_MAGIC, // !< Packetized telemetry packet type
                FW_PACKET_IDLE = IDLE_MAGIC, // !< Idle packet
                FW_PACKET_UNKNOWN = UNKNOWN_MAGIC // !< Unknown packet
            } ComPacketType;

            ComPacket();
            virtual ~ComPacket();

        protected:
            ComPacketType m_type;
            SerializeStatus serializeBase(SerializeBufferBase& buffer) const ; // called by derived classes to serialize common fields
            SerializeStatus deserializeBase(SerializeBufferBase& buffer); // called by derived classes to deserialize common fields
    };

} /* namespace Fw */

#endif /* COMPACKET_HPP_ */
