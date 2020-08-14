/*
 * All Acceptable States for a waypoint segment.
 */

import Enum from './Enum.js'

export default {
  Waypoint: new Enum(
    "VISITED", //    - Command to this waypoint was executed
    "UNVISITED", //  - Command to this waypoint has not been executed
    "EDITING",   //  - Waypoint is being edited
  ),
};
