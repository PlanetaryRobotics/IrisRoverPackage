'use strict';
/*
 * Defines common datastructure elements used by all Telemetry data.
 * Should serve as a base class for other telemetry datatypes. Should not be instantiated directly.
 *
 * Author: Connor W. Colombo, CMU
 * Created: 08/25/2020
 * Last Update: 08/28/2020, Colombo
 */

import DBObject from '../DBObject.js'

export default class TelemetryData extends DBObject{
  constructor(inputData){
     // Default values:
     super(inputData, { // data which gets saved to JSON
     // Data EVERY Telemetry Object Should Contain (in addition to DBObject default data):
        time: {
          collected: DBObject.NULL_TIME, // - DateTime this data was collected on the rover
          sent: DBObject.NULL_TIME, //      - DateTime this data was sent from the rover
          received: DBObject.NULL_TIME //   - DateTime this data was received by ground
        }
      });

      if(this.constructor.name === "TelemetryData"){
        console.error(`TelemetryData is supposed to serve as a base class for other Telemetry data types and 
                       should not be instantiated directly.`);
      }
  } // ctor

  // Loads Object from JSON.
  static fromJSON(data){
    if(typeof data == "string"){
      data = JSON.parse(data);
    }
    // Validate Data Types:
    data.time.sent = this.luxonFromJSON(data.time.sent);
    data.time.received = this.luxonFromJSON(data.time.received);

    return new TelemetryData(data);
  } // #fromJSON

 } // class: TelemetryData
