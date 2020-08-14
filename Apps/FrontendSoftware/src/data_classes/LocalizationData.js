'use strict';
/*
 * Defines Standard Form for Data that Represents the Rover's Location after
 * Executing a Given Command as Determined by Localization.
 * Allows for Consistent Expectations when Passing Data between Mongo and
 * Frontend UI.
 *
 * This is used to not only represent that position updates which follow the
 * arrival of a new image; but also, intermittent position updates which come as
 * a result of integrating higher frequency but less reliable sensor
 * measurements (eg. IMU or odometry data).
 *
 * Author: Connor W. Colombo, CMU
 * Last Update: 06/05/2020, Colombo
 */

//import Enum from './Enum.js'
import DBObject from './DBObject.js'

export default class LocalizationData extends DBObject{
  constructor(inputData){
    super(inputData);
  } // ctor

  // Returns the Default Data this Object Should Contain
  static defaultData(){
    return { // data which gets saved to JSON
      collectionTime: false, // Time the source data (image) was collected.
      sendTime: false, // Time this packet was completed and pushed to DB.

      // Note: for positions x+ points to the right on the map and y+ points up.
      position: [0,0,0], // Position estimate [x,y, heading]
      uncertainty: [0,0,0], // Uncertainty in the measurement [+-x, +-y, +-angle]

      // These fields are only used if this packet corresponds to / is produced
      // from an image. commandLookupID is 0 if this data did not come while any
      // commands were being executed.
      commandLookupID: -1, // ID of Command which put the rover in this state

      // Moves longer than frame.len will get broken up into multiple smaller
      // moves for a breadcrumb image to be taken after each one.
      // Note: n and tot count from 1. n is 0 if this data does not correspond
      // with any image-bearing point (ie. end of a command or a breadcrumb frame)
      frame: {
        len: 10, // [cm] Length of segmenting
        n: 0, // Frame number corresponding to this data point
        tot: 0 // Total number of frames in this move
      }
    }
  }

  // Loads Object from JSON.
  static fromJSON(data){
    if(typeof data == "string"){
      data = JSON.parse(data);
    }
    // Validate Data Types:
    data.collectionTime = data.collectionTime ? new Date(data.collectionTime) : false;
    data.sendTime = data.sendTime ? new Date(data.sendTime) : false;
    return new LocalizationData(data);
   } // #fromJSON

   // Used by JSON.stringify
   toJSON(){
     let jsonObject = Object.assign({}, this.data); // make a copy
     return JSON.stringify(jsonObject);
   } // #toJSON
 } // class: CommandData
