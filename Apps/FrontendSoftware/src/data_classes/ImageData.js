'use strict';
/*
 * Defines Standard Form for Data that Should be Contained by an Image. Allows
 * for Consistent Expectations when Passing Data between Mongo and Frontend UI.
 * Author: Connor W. Colombo, CMU
 * Last Update: 10/23/2019, Colombo
 */

import fs from 'fs'
import path from 'path'
import DB from '@/DBInterface/DBInterface.js'

import DBObject from './DBObject.js'

const ENCODING = "jpeg"; // Encoding type used for all image objects

export default class ImageData extends DBObject{
  constructor(inputData){
    super(inputData);
    this.file_data = Buffer.from([]); // pre-populate with empty buffer for Vue to reactively hook onto.
    this.loadData(); // asynchronously preload any image data referenced but not supplied
  } // ctor

  // Returns the Default Data this Object Should Contain
  static defaultData(){
    return { // data which gets saved to JSON
      lookupID: -1,
      name: 'Bad Image',
      bin: [], // Uint8Array of bytes of image data. 'url' or 'bin' must be supplied. URL must be local (due to same-origin policy).
      file: {}, // MongoDB Object idenifier for file location in GridFS.
      camera: "none",
      sendTime: false,
      commandLookupID: "no command",
      tags: []
    }
  }

  // Loads Object from JSON string.
  static fromJSON(data){
    if(typeof data == "string"){
      data = JSON.parse(data);
    }
    // Validate Data Types:
    data.sendTime = data.sendTime ? new Date(data.sendTime) : false;
    return new ImageData(data);
  } // #fromJSON

  // Used by JSON.stringify
  toJSON(){
    let jsonObject = Object.assign({}, this.data);
    return JSON.stringify(jsonObject);
  } // #toJSON

  // Returns the URL in 'data'. If this was not given, it creates a URI from
  // the binary data given in 'data'.
  get url(){
    if(this.data.url){
     return this.data.url;
    } else if(this.data.bin && this.data.bin.length > 0){
     return `data:image/${ENCODING};base64,` + this.data.bin.toString('base64');
    } else{
     return ""; // no valid image data exits
    }
  }

   // Returns the Binary Image Data from 'data'. If this was not given,
   // it creates one from the file found at the 'url' given in 'data'.
   get bin(){
     // Find available data source to return:
     if(this.file_data){ // data has been read from file
       return this.file_data;
     } else if(this.data.bin){ // file shipped with binary data
       return this.data.bin;
     } else{
       return ""; // no valid image data exits
     }
   }

   // Helper function to await load data of file (is local or DB from GridFS), if
   // applicable.
   async loadData(){
     if(
       Object.keys(this.data.file).length // if MongoDB file address is given.
       && DB.connectionEstablished()
       && (!this.file_data || !Object.keys(this.file_data).length) // and file data hasn't been populated yet
     ){
       let data = await DB.readFullFile(this.data.file);
       this.file_data = Buffer.from(data);
     } else if(this.data.url){
       let data = fs.readFileSync(this.data.url);
       this.file_data = Buffer.from(Uint8Array.from(data));
     }
   }

   /*
    * Creates a Filename to be used when this Image is Downloaded. Filename will
    * take the form:
    * FILENAME-CAMERA-lookupID-CommandLookupID-unixTimestamp
   **/
   get fileName(){
     let metaData = [
       this.data.name,
       this.data.camera,
       this.data.lookupID,
       this.data.commandLookupID,
       this.data.sendTime.getTime()
     ];
     return metaData.join('-') + "." + ENCODING;
   }

   /* Saves this image into the given (full path) directory. */
   save(dir){
     return fs.writeFileSync(path.join(dir, this.fileName), this.bin);
   }

   // Returns a Nicely Formatted Timestamp
   get time(){
     return this.data.sendTime ? this.data.sendTime.toLocaleDateString('en-US', { timezone: 'UTC', hour12 : false, day: '2-digit', hour: '2-digit', minute: '2-digit', second: '2-digit'}) : "";
   }

   // Used for ease of sorting by time in UI
   get timeObject() {
     return this.data.sendTime ? new Date(Date.now()) : ""; 
   }

   name(){
     return this.data.name
   }
 } // class: ImageData
