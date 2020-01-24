'use strict';
/*
 * Defines Standard Form for How Data Passed In and Out of the Database should
 * be Structured and Accessed. Allows for Consistent Expectations when Passing
 * Data between Mongo and Frontend UI.
 * Author: Connor W. Colombo, CMU
 * Last Update: 5/8/2019, Colombo
 */
 // TODO: Find a more efficient way to imp. #plainJSON. Not big issue, negligible perf. debt at the moment.

import { sha256 } from 'js-sha256'

export default class DBObject{
   constructor(inputData){
     // Default values:
     this.data = this.constructor.defaultData();
     Object.assign(this.data, inputData); // override default values
   } // ctor

   // Returns the Default Data this Object Should Contain
   static defaultData(){
     return { // data which gets saved to JSON
       lookupID: -1
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
