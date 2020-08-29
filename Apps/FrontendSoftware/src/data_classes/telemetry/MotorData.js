'use strict';
/*
 * Defines the data contained by an individual motor in TelemetryData_Motor.
 *
 * Author: Connor W. Colombo, CMU
 * Created: 08/25/2020
 * Last Update: 08/28/2020, Colombo
 */

export default class MotorData{
  constructor({
    encoder_dist = 0,
    current = 0,
    speed = 0
  } = {}){
    this.encoder_dist = encoder_dist;
    this.current = current;
    this.speed = speed;
  } // ctor
 } // class: MotorData
