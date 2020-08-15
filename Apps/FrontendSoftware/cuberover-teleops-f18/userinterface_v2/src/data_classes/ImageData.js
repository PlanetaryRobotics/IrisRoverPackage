'use strict';
/*
 * Defines Standard Form for Data that Should be Contained by an Image. Allows
 * for Consistent Expectations when Passing Data between Mongo and Frontend UI.
 * Author: Connor W. Colombo, CMU
 * Last Update: 5/12/2019, Colombo
 */

import fs from 'fs'

import DBObject from './DBObject.js'

export default class ImageData extends DBObject{
  constructor(inputData){
    super(inputData);
  } // ctor

  // Returns the Default Data this Object Should Contain
  static defaultData(){
    return { // data which gets saved to JSON
      name: 'Bad Image',
      bin: [], // Uint8Array of bytes of image data. 'url' or 'bin' must be supplied. URL must be local (due to same-origin policy).
      camera: "none",
      sendTime: false,
      command: "no command",
      commandDesc: "",
      tags: []
    }
  }

  // Loads Object from JSON string.
  fromJSON(str){
    let data = JSON.parse(str);
    // Validate Data Types:
    data.sendTime = data.sendTime ? new Date(data.sendTime) : false;
    return new ImageData(data);
  } // #fromJSON

  // Used by JSON.stringify
  toJSON(){
    let jsonObject = Object.assign({}, this.data);
    jsonObject.bin = this.bin; // Encode image found at 'data.url' if 'data.bin' not given.
    jsonObject.url = undefined; // Remove the url reference from the JSON
    return JSON.stringify(jsonObject);
  } // #toJSON

  // Returns the URL in 'data'. If this was not given, it creates a URI from
  // the binary data given in 'data'.
  get url(){
    if(this.data.url){
     return this.data.url;
    } else if(this.data.bin){
     return 'data:image/jpeg;base64,' + Buffer.from(this.data.bin).toString('base64');
    } else{
     return ""; // no valid image data exits
    }
  }

   // Returns the Binary Image Data from 'data'. If this was not given,
   // it creates one from the file found at the 'url' given in 'data'.
   get bin(){
     if(this.data.bin){
       return this.data.bin;
     } else if(this.data.url){
       let data = fs.readFileSync(this.data.url);
       return Uint8Array.from(data);
     } else{
       return ""; // no valid image data exits
     }
   }

   // Returns a Nicely Formatted Timestamp
   get time(){
     return this.data.sendTime ? this.data.sendTime.toLocaleDateString('en-US', { timezone: 'UTC', hour12 : false, day: '2-digit', hour: '2-digit', minute: '2-digit', second: '2-digit'}) : "";
   }

   name(){
     return this.data.name
   }
 } // class: ImageData
