#include "../PrimaryFlightController/FlightMCU/Include/CubeRoverConfig.hpp"

#ifndef _CUBEROVER_FPRIME_CONFIG_HPP_
#define _CUBEROVER_FPRIME_CONFIG_HPP_


/* Place overrides of the default FPrime configs here */

#define FwOpcodeType U16         // Use smaller field for storing opcodes
#define FwChanIdType U16         // Use smaller field for storing tlm channels
#define FwEventIdType U16        // Use smaller field for storing log Ids
#define FwPrmIdType U16          // Use smaller field for storing parameter ids


// Don't include time base or context when serializing Fw::Time
#define FW_USE_TIME_BASE                0
#define FW_USE_TIME_CONTEXT             0


/* Other parameterized modifications to FPrime core are placed below with 
 * a commend to where the modification was made unless otherwise noted
 */


// Fw/Com/ComPacket.hpp:ComPacketType
// Change default packet types to magic values for more deterministic evaluation
// These are enumerations -> int -> 32bit values required
#define COMMAND_MAGIC           FSW_COMMAND_MAGIC
#define LOG_MAGIC               FSW_LOG_MAGIC
#define TELEMETRY_MAGIC         FSW_TELEMETRY_MAGIC
#define FILE_MAGIC              FSW_FILE_MAGIC
// Magic for unused(?) packet types
#define PACKETIZED_TLM_MAGIC    0x8badb33f
#define IDLE_MAGIC              0xcafef00d
#define UNKNOWN_MAGIC           0x4b1d4b1d
// Additional packet magic
#define COMMAND_RESPONSE_MAGIC  0xc001d00d

#endif // _CUBEROVER_FPRIME_CONFIG_HPP_

