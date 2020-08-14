'use strict';
/*
 * Defines Standard Form for How Data Passed In and Out of the Database should
 * be Structured and Accessed. Allows for Consistent Expectations when Passing
 * Data between Mongo and Frontend UI.
 * Author: Connor W. Colombo, CMU
 * Last Update: 08/14/2020, Colombo
 */
 // TODO: Find a more efficient way to imp. #plainJSON. Not big issue, negligible perf. debt at the moment.

import { sha256 } from 'js-sha256'
import { v1 as uuidv1 } from 'uuid';

export default class DBObject{
   constructor(inputData){
     // Default values:
     this.data = DBObject.defaultData();
     Object.assign(this.data, this.constructor.defaultData()); // override any DBObject defaults with extending class defaults
     Object.assign(this.data, inputData); // override defaults with given values
   } // ctor

   // Returns the Default Data that EVERY DBObject Should Contain
   static defaultData(){
     return { // data which gets saved to JSON
       lookupID: -1,
       uuid: uuidv1()
     }
   }

   // Loads Object from JSON.
   static fromJSON(data){
     if(typeof data == "string"){
       let data = JSON.parse(data);
     }
     return new DBObject(data);
   } // #fromJSON

   // Returns a SHA-256 Hash of Plain JSON Contents of this Object.
   get hash(){
     return sha256(this.toJSON());
   }

   /**
    * Returns the database assigned `_id` as a string (if this object has been on the DB).
    * Otherwise, returns the locally assigned `lookupID` as string.
    */
   get DBID(){
    if(this.data._id){
      return this.data._id.id.join(':'); 
    } else {
      return this.data.lookupID.toString();
    }
   }

   // Returns a plain JSON object representing the core data of this instance
   // (using primitives only)
   plainJSON(){
     return JSON.parse(this.toJSON()); // this method filters out all of Vue's reactive getters/setters
   } // #plainJSON

   // Used by JSON.stringify
   toJSON(){
     return JSON.stringify(this.data);
   } // #toJSON
 } // class: CommandData
