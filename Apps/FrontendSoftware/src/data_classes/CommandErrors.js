/*
 * All Acceptable Errors for Commands Sent to the Rover.
 */

import Enum from './Enum.js';

export default {
    // Technical Errors in Commands Sent to Rover (ie, as FPrime is concerned):
    Fp: new Enum(
        'TRANSMISSION', //  - Command receipt not acknowledged by rover
        'EXECUTION' //      - Command not executed properly by rover
    ),
    // User-Interface related errors:
    UI: new Enum(
        'DB_NOCONNECT', //- Application could not connect to database server.
        'TIMEOUT', //     - Command sent, took too long to get a response
        'PERMISSION', //  - Command was NOT sent. User does not have permission to issue commands.
    )
};
