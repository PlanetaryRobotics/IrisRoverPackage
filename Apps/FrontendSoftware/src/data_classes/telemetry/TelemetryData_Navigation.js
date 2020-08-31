'use strict';
/*
 * Defines datastructure for Telemetry coming from the Navigation Module.
 *
 * Author: Connor W. Colombo, CMU
 * Created: 08/25/2020
 * Last Update: 08/28/2020, Colombo
 */

import TelemetryData from './TelemetryData.js';

export default class TelemetryData_Navigation extends TelemetryData{
  constructor(inputData){
    super(inputData);
  } // ctor

  // Returns the Default Data this Object Should Contain
  static defaultData(){
    return { // data which gets saved to JSON
      distMovedForward: 0, //       - Distance the rover moved forward
      distMovedBackward: 0, //      - Distance the rover moved backward
      angleTurnedRight: 0, //       - Angle the rover turned right
      angleTurnedLeft: 0, //        - Angle the rover turned left
      imageSize: 0 //                - Size of image that was taken during the move
    }
  }

  // Loads Object from JSON.
  static fromJSON(data){
    // Process all standard telemetry data first:
    data = (new TelemetryData(data)).data;

    return new TelemetryData_Navigation(data);
  } // #fromJSON

 } // class: TelemetryData_Navigation
