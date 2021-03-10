'use strict';
/*
 * Defines Standard Form for Data that Should be Contained by a Command. Allows
 * for Consistent Expectations when Passing Data between Mongo and Frontend UI.
 * Author: Connor W. Colombo, CMU
 * Last Update: 7/08/2019, Colombo
 */
// { "_id" : ObjectId("5d0ebc51efd2a534837045aa"), "args" : { "Distance" : 200, "Images" : 60, "Speed" : 99 }, "lookupID" : 81, "name" : "MoveBackward", "stateFp" : "SUCC_SENT" }

import Enum from './Enum.js';
import States from './CommandStates.js';
import Errors from './CommandErrors.js';
import DBObject from './DBObject.js';

export default class CommandData extends DBObject{
    constructor(inputData){
        super(inputData);
    } // ctor

    // Returns the Default Data this Object Should Contain
    static defaultData(){
        return { // data which gets saved to JSON
            name: '',
            args: {
                Images: 1, // number of images requested by command
            },
            sendTime: false,
            stateFp: States.Fp.NOT_A_TYPE,
            stateUI: States.UI.NOT_A_TYPE,
            errorFp: Errors.Fp.NOT_A_TYPE,
            errorUI: Errors.UI.NOT_A_TYPE,
            waypointData: null
        };
    }

    // Loads Object from JSON.
    static fromJSON(data){
        if(typeof data == 'string'){
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
