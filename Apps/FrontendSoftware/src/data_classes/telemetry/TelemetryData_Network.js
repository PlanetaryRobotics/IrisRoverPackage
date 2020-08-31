'use strict';
/*
 * Defines datastructure for Telemetry coming from the Network Monitor Module.
 *
 * Author: Connor W. Colombo, CMU
 * Created: 08/25/2020
 * Last Update: 08/28/2020, Colombo
 */

import TelemetryData from './TelemetryData.js';

export default class TelemetryData_Network extends TelemetryData{
  constructor(inputData){
    super(inputData);
  } // ctor

  // Returns the Default Data this Object Should Contain
  static defaultData(){
    return { // data which gets saved to JSON
      packet_count: {
        received: 0, //   - Number of packets received by rover
        sent: 0, //       - Number of packets sent by rover
      },
      rssi: 0, //         - RSSI in [dB]
      bytes: {
        received: 0, //   - Bytes received by rover
        sent: 0 //        - Bytes sent by rover
      },
      error: 0 //         - Packets error
    }
  }

  // Loads Object from JSON.
  static fromJSON(data){
    // Process all standard telemetry data first:
    data = (new TelemetryData(data)).data;

    return new TelemetryData_Network(data);
  } // #fromJSON

 } // class: TelemetryData_Network
