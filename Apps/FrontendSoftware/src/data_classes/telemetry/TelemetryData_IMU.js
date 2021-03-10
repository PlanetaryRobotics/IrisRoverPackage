'use strict';
/*
 * Defines datastructure for Telemetry coming from the IMU Module.
 *
 * Author: Connor W. Colombo, CMU
 * Created: 08/25/2020
 * Last Update: 08/28/2020, Colombo
 */

import TelemetryData from './TelemetryData.js';

export default class TelemetryData_IMU extends TelemetryData{
    constructor(inputData){
        super(inputData);
    } // ctor

    // Returns the Default Data this Object Should Contain
    static defaultData(){
        return { // data which gets saved to JSON
            accel: {
                x: 0,
                y: 0,
                z: 0
            },
            angle: {
                x: 0,
                y: 0,
                z: 0
            }
        };
    }

    // Loads Object from JSON.
    static fromJSON(data){
    // Process all standard telemetry data first:
        data = (new TelemetryData(data)).data;

        return new TelemetryData_IMU(data);
    } // #fromJSON

} // class: TelemetryData_IMU
