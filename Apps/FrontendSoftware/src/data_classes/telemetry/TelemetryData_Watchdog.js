'use strict';
/*
 * Defines datastructure for Telemetry coming from the Watchdog Module.
 *
 * Author: Connor W. Colombo, CMU
 * Created: 08/25/2020
 * Last Update: 08/28/2020, Colombo
 */

import TelemetryData from './TelemetryData.js';

export default class TelemetryData_Watchdog extends TelemetryData{
    constructor(inputData){
        super(inputData);
    } // ctor

    // Returns the Default Data this Object Should Contain
    static defaultData(){
        return { // data which gets saved to JSON
            hardware_ok: false,

            power: {
                voltage: {
                    voltage_ok: {
                        V1V2: false,
                        V1V8: false,
                        V3V3: false,
                        V5V0: false
                    },
                    V2V5: 0,
                    V2V8: 0,
                    V24V: 0,
                    V28V: 0
                },
  
                current: {
                    rail3V3: 0, // total current on the 3V3 rail
                    hercules_3V3: 0, // on 3V3 rail
                    hercules_1V2: 0, // on 1V2 rail
                    FPGA_3V3: 0, // on 3V3 rail
                    FPGA_1V2: 0, // on 1V2 rail
                    radio: 0
                }
            },

            temperature: {
                hercules: 0,
                FPGA: 0,
                radio: 0,
                battery: 0,
                camera: {
                    front: 0,
                    rear: 0
                },
                motor: [
                    0, // Motor 0
                    0, // Motor 1
                    0, // Motor 2
                    0 //  Motor 3
                ]
            }
        };
    }

    // Loads Object from JSON.
    static fromJSON(data){
    // Process all standard telemetry data first:
        data = (new TelemetryData(data)).data;

        return new TelemetryData_Watchdog(data);
    } // #fromJSON

} // class: TelemetryData_Watchdog
