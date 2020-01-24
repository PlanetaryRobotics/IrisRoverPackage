'use strict';
/*
 * Defines Standard Form for Data that Should be Contained by a Command. Allows
 * for Consistent Expectations when Passing Data between Mongo and Frontend UI.
 * Author: Connor W. Colombo, CMU
 * Last Update: 5/11/2019, Colombo
 */

import Enum from './Enum.js'
import States from './CommandStates.js'
import Errors from './CommandErrors.js'
import DBObject from './DBObject.js'

export default class CommandData extends DBObject{
  constructor(inputData){
    super(inputData);
  } // ctor

  // Returns the Default Data this Object Should Contain
  static defaultData(){
    return { // data which gets saved to JSON
      lookupID: -1,
      name: "",
      args: {}, // Should look like: {distance: 10, speed: 5}
      sendTime: false,
      stateFp: States.Fp.NOT_A_TYPE,
      stateUI: States.UI.NOT_A_TYPE,
      errorFp: Errors.Fp.NOT_A_TYPE,
      errorUI: Errors.UI.NOT_A_TYPE
    }
  }

  // Loads Object from JSON.
  static fromJSON(data){
    if(typeof data == "string"){
      data = JSON.parse(data);
    }
     // Validate Data Types:
     data.sendTime = data.sendTime ? new Date(data.sendTime) : false;
     data.stateFp = States.Fp.fromString(data.stateFp);
     data.stateUI = States.UI.fromString(data.stateUI);
     data.errorFp = Errors.Fp.fromString(data.errorFp);
     data.errorUI = Errors.UI.fromString(data.errorUI);
     return new CommandData(data);
   } // #fromJSON

   // Used by JSON.stringify
   toJSON(){
     let jsonObject = Object.assign({}, this.data); // make a copy
     jsonObject.stateFp = Enum.toString(jsonObject.stateFp);
     jsonObject.stateUI = Enum.toString(jsonObject.stateUI);
     jsonObject.errorFp = Enum.toString(jsonObject.errorFp);
     jsonObject.errorUI = Enum.toString(jsonObject.errorUI);
     return JSON.stringify(jsonObject);
   } // #toJSON
 } // class: CommandData
