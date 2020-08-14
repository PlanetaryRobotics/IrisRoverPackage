/*
 * All Acceptable States for Commands Sent to the Rover.
 */

import Enum from './Enum.js'

export default {
  // Technical State of Command Sent to Rover (ie, as FPrime is concerned):
  Fp: new Enum(
    "NOT_SENT", //    - No Fprime Data b/c Command Not Sent Yet
    "SUCC_SENT", //   - Successfully sent to rover
    "SUCC_EXEC", //   - Successfully executed by rover
    "FAIL" //         - Command failed. See associated error.
  ),
  // Display state of command sent to rover in UI:
  UI: new Enum(
    "LOCAL_ONLY", //  - Command not yet sent to database. If this machine exits program, command is lost.
    "WAITING", //     - Command sent, state unknown
    "SUCCESS", //     - Command sent to and executed successfully by rover
    "FAIL" //         - Command failed in someway. See associated error.
  )
};
