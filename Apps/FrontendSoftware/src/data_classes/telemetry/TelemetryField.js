'use strict';
/*
 * Contains Metadata about a Telemetry Field
 *
 * Author: Connor W. Colombo, CMU
 * Created: 08/29/2020
 * Last Update: 08/29/2020, Colombo
 */

export default class TelemetryField{
  constructor({
    name = console.error("Must give name for telemetry field"),
    units = "",
    
  } = {}){
    this.name = name;
    this.units = units;
  } // ctor
 } // class: MotorData
