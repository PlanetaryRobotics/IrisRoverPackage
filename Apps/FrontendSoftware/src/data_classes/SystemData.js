'use strict';
/*
 * Defines datastructure for representing and managing the current state of the entire 
 * software system.
 * Metadata about connection between the backend and avionics.
 * Definitions for Clocks relative to all major events.
 *
 * Author: Connor W. Colombo, CMU
 * Created: 08/07/2020
 * Last Update: 08/28/2020, Colombo
 */

const { DateTime } = require('luxon');

import DBObject from './DBObject.js'
import Clock from '@/system/Clock.js'

export default class SystemData extends DBObject{
  constructor(inputData){
    super(inputData);
  } // ctor

  // Returns the Default Data this Object Should Contain
  static defaultData(){
    return { // data which gets saved to JSON
      backend: {
        connectedToRover: false, //     - Whether the backend has a connection to the rover.
      },
      avionics: {
        deployed: false, //             - Whether a deployment signal has been sent to the rover
        lastMessageTime: undefined, //  - (Luxon) DateTime of last message from the rover received by backend
        mode: { //                      - Data about the Avionics Mode (TODO:(colombo@cmu.edu): from FPrime Mode Manager?)
          currMode: 0,  //                -- Current Mode
          prevMode: 0  //                 -- Previous Mode
        },
        health: {//                     - Avionics Health (from FPrime Health Message)
          modules_ok: false //            -- Whether FPrime modules are in an error state (1 -> ERROR, 0 -> OK)
        }
      },
      timestamp: DateTime.utc(), //     - DateTime this system state was recorded.
      time: {
          // NOTE: The time offsets used here are arbitrary and should be updated on the DB using an inspection tool (eg. Atlas) once known.
          launch: new Clock({ code: "L", name: "Launch", zeroTime: DateTime.utc().minus({days: 18}) }),
          landing: new Clock({ code: "LL", name: "Lunar Landing", zeroTime: DateTime.utc().minus({hours: 4}) }),
          moon: new Clock({ code: "M", name: "Moon", zeroTime: DateTime.utc() })
      }
    }
  }

  // Loads Object from JSON.
  static fromJSON(data){
    if(typeof data == "string"){
      data = JSON.parse(data);
    }
    // Validate Data Types:
    data.avionics.lastMessageTime = this.luxonFromJSON(data.avionics.lastMessageTime);
    data.timestamp = this.luxonFromJSON(data.timestamp);

    data.time.launch = Clock.fromJSON(data.time.launch);
    data.time.landing = Clock.fromJSON(data.time.landing);
    data.time.moon = Clock.fromJSON(data.time.moon);

    return new SystemData(data);
  } // #fromJSON

 } // class: SystemData
