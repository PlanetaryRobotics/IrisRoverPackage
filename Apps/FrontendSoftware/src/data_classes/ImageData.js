'use strict';
/*
 * Defines Standard Form for Data that Should be Contained by an Image. Allows
 * for Consistent Expectations when Passing Data between Mongo and Frontend UI.
 * Author: Connor W. Colombo, CMU
 * Created: 2019
 * Last Update: 08/30/2020, Colombo
 */

/* global __static */ // <- keep eslint from complaining about the __static directory
import path from 'path';
import fs from 'fs';
import DB from '@/DBInterface/DBInterface.js';

import { toUint8Array as Base64ToUint8Array } from 'js-base64';

import utils from '@/utils.js';

import DBObject from './DBObject.js';

const ENCODING = 'jpeg'; // Encoding type used for all image objects

// Load graphic shown while the image is loading:
const defaultImage = { utils };
utils.readFilePromise(path.join(__static, './default_states/ImageLoading.png')).then( data => {
    defaultImage.file = data;
    utils.readFileAsBytes(data).then( data => defaultImage.bin = data );
    utils.readImageAsBase64URL(data).then( data => defaultImage.url = data );
});
// Load graphic shown if image failed to load:
const errorImage = {};
utils.readFilePromise(path.join(__static, './default_states/ImageError.png')).then( data => {
    errorImage.file = data;
    utils.readFileAsBytes(data).then( data => errorImage.bin = data );
    utils.readImageAsBase64URL(data).then( data => errorImage.url = data );
});

export default class ImageData extends DBObject{
  static defaultImage = defaultImage;
  static errorImage = errorImage;

  loading = true; // Whether this image's source file is still being loaded.
  loadingFailed = false; // Whether data loading was attempted and failed

  constructor(inputData){
      super(inputData);
      this.file_data = Buffer.from([]); // pre-populate with empty buffer for Vue to reactively hook onto.
      this.loadData(); // asynchronously preload any image data referenced but not supplied
  } // ctor

  // Returns the Default Data this Object Should Contain
  static defaultData(){
      return { // data which gets saved to JSON
          name: 'Bad Image',
          bin: [], // Uint8Array of bytes of image data. 'url' or 'bin' must be supplied. URL must be local (due to same-origin policy).
          file: {}, // MongoDB Object identifier for file location in GridFS.
          camera: 'none',
          sendTime: false,
          commandLookupID: 'no command',
          tags: []
      };
  }

  // Loads Object from JSON string.
  static fromJSON(data){
      if(typeof data == 'string'){
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
      if(this.data.url && this.data.url.length > 0){
          return this.data.url;
      } else if(this.file_data && this.file_data.length > 0){
          return `data:image/${ENCODING};base64,` + this.file_data.toString('base64');
      } else if(this.data.bin && this.data.bin.length > 0){
          return `data:image/${ENCODING};base64,` + this.data.bin.toString('base64');
      } else {
          // no valid image data exists, return appropriate default image:
          return this.loading ? ImageData.defaultImage.url : ImageData.errorImage.url;
      }
  }

  // Returns the Binary Image Data from 'file_data' or 'data'. If this was not given
  // it creates one from the file found at the 'url' given in 'data'.
  get bin(){
      // Find available data source to return:
      if(this.file_data && this.file_data.length > 0){ // data has been read from file
          return this.file_data;
      } else if(this.data.bin && this.data.bin.length > 0){ // file shipped with binary data
          return this.data.bin;
      } else if(this.data.url && this.data.url.length > 0){
          return Buffer.from(Base64ToUint8Array(this.data.url));
      } else {
          // no valid image data exists, return appropriate default image:
          return this.loading ? ImageData.defaultImage.bin : ImageData.errorImage.bin;
      }
  }

  /** 
    * Whether this image couldn't be successfully loaded.
    * Returns true if invalid data was provided for this image or if data loading failed.
    */
  get failed() {
      return this.loadingFailed
      || !this.loading // Not currently loading
      && !(this.file_data && this.file_data.length > 0) // No data has been loaded
      && !(this.data.bin && this.data.bin.length > 0) // No valid Buffer was supplied
      && !(this.data.url && this.data.url.length > 0); // No valid data url was supplied
  }

  // Helper function to await load data of file (is local or DB from GridFS), if
  // applicable.
  async loadData(){
      try{
          if(
              Object.keys(this.data.file).length // if MongoDB file address is given:
          && (!this.file_data || Object.keys(this.file_data).length === 0) // and file data hasn't been populated yet
          ){
              this.loading = true;
              await DB.forceAction(async () => {
                  let data = await DB.readFullFile(this.data.file);
                  this.file_data = Buffer.from(data);
              });
          } else if(this.data.url){
              this.loading = true;
              let data = await utils.readFilePromise(this.data.url);
              this.file_data = Buffer.from(Uint8Array.from(data));
          }
      } catch(err){
          this.loadingFailed = true;
          console.error(`[IRIS-DB-IMAGEDATA] Failed to load data for image with lookupID ${this.data.lookupID}. Errored out with: `, err);
      }
      
      this.loading = false;
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
      return metaData.join('-') + '.' + ENCODING;
  }

  /* Saves this image into the given (full path) directory. */
  save(dir){
      return fs.writeFileSync(path.join(dir, this.fileName), this.bin);
  }

  // Returns a Nicely Formatted Timestamp
  get time(){
      return this.data.sendTime ? this.data.sendTime.toLocaleDateString('en-US', { timezone: 'UTC', hour12 : false, day: '2-digit', hour: '2-digit', minute: '2-digit', second: '2-digit'}) : '';
  }

  // Used for ease of sorting by time in UI
  get timeObject() {
      return this.data.sendTime ? this.data.sendTime : ''; 
  }

  get timeForTagFormatting() {
      let obj = this.data.sendTime;

      let month = obj.getUTCMonth() + 1; 
      let day = obj.getUTCDate();
      let hours = obj.getHours();
      let min = obj.getMinutes();

      function addZero(x) {
          if (x.toString().length === 1) {
              return '0' + x;
          }
          return x;
      }

      return addZero(month) + '-' + addZero(day) + ' ' + addZero(hours) + ':' + addZero(min);
  }

  name(){
      return this.data.name;
  }
} // class: ImageData
