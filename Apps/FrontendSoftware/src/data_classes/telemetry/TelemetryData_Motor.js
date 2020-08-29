'use strict';
/*
 * Defines datastructure for Telemetry coming from the Motor Interface Module.
 *
 * Author: Connor W. Colombo, CMU
 * Created: 08/25/2020
 * Last Update: 08/28/2020, Colombo
 */

import TelemetryData from './TelemetryData.js';
import MotorData from './MotorData.js';

export default class TelemetryData_Motor extends TelemetryData{
  constructor(inputData){
    super(inputData);
  } // ctor

  // Returns the Default Data this Object Should Contain
  static defaultData(){
    return { // data which gets saved to JSON
      motors: [
        new MotorData(),
        new MotorData(),
        new MotorData(),
        new MotorData()
      ]
    }
  }

  // Loads Object from JSON.
  static fromJSON(data){
    // Process all standard telemetry data first:
    data = (new TelemetryData(data)).data;

    if(data.motors){
      for(let i=0; i<4; i++){
        if(data.motors[i]){
          data.motors[i] = new MotorData(data.motors[i]);
        }
      }
    }

    return new TelemetryData_Motor(data);
  } // #fromJSON

 } // class: TelemetryData_Motor
