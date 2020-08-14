'use strict';
/*
 * Defines datastructure for representing and managing the current state of the entire 
 * software system.
 * Metadata about connection between the backend and avionics.
 * Definitions for Clocks relative to all major events.
 *
 * Author: Connor W. Colombo, CMU
 * Created: 08/07/2020
 * Last Update: 08/07/2020, Colombo
 */

const { DateTime } = require('luxon');

import DBObject from './DBObject.js'
import Clock from '@/system/Clock.js'

/**
 * Returns a luxon DateTime object constructed from the given json string
 * if the given json string is valid.
 */
function luxonFromJSON(json){
  return json ? DateTime.fromISO(json) : json;
}

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
    data.avionics.lastMessageTime = luxonFromJSON(data.avionics.lastMessageTime);
    data.timestamp = luxonFromJSON(data.timestamp);

    data.time.launch = Clock.fromJSON(data.time.launch);
    data.time.landing = Clock.fromJSON(data.time.landing);
    data.time.moon = Clock.fromJSON(data.time.moon);

    return new SystemData(data);
  } // #fromJSON

 } // class: SystemData
