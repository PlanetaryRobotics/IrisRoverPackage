'use strict';
/*
 * Defines Standard Form for Data that Should be Contained by a Filter.
 * This allows for easy and consistent formatting of Filters.
 *
 * An example of a Filter is:
 * Filter({
    name = "FilterName",
    adjustments: {
      exposure:   {value: -50,  defaultVal: 0},
      contrast:   {value: 20,   defaultVal: 0},
      highlights: {value: 10,   defaultVal: 0},
      shadows:    {value: -10,  defaultVal: 0},
      whites:     {value: 0,    defaultVal: 0},
      blacks:     {value: 30,   defaultVal: 0},
    },
    initialImage: "CR_M01_0001"
  })
  *
  * Author: John Walker Moosbrugger, CMU
  * Created: 4/6/2019
  * Last Update: 11/1/2019, Colombo
 */

export default class Preset{
  constructor(inputData){
    /* Constructs a new Filter from the given data.
      The format of params is an array of objects containing the elements:
      { filterName, adjustments as an object with keys for each parameter }.
    */
    this.data = {
      name: "",
      adjustments: {
        Exposure: 0,
        Contrast: 0,
        Denoise: 0,
        Shadows: 0
      },
      imageList: [],
      global: false,
      shared: false
    }

    // Validates that filterName is a valid input
    if ( inputData.name === "" || typeof inputData.name != "string" ) {
      console.error("Invalid filterName in Filter Constructor. Name is empty or not a string.");
      console.log(inputData.name);
    }
    else if ( inputData.name.length > 30 ) {
      console.error("Invalid filterName in Filter Constructor. Name is too long.")
      console.log(inputData.name)
    }

    // Validates all expected adjustments are present and within acceptable ranges
    let adjustmentInputs = Object.keys(this.data.adjustments); // expected adjustment inputs

    for(let index in adjustmentInputs){
      let param = adjustmentInputs[index]
      if(!(
        param in inputData.adjustments
        && typeof inputData.adjustments[param] === "number"
        && inputData.adjustments[param] > -1
        && inputData.adjustments[param] < 1
      )){
        console.error("Invalid Format of The Following Parameter Given to CommandOption Constructor.");
        console.log(inputData.adjustments.param);
      }
    }

    if (inputData.initialImage) { // if image is valid
      this.data.imageList.push(inputData.initialImage)
    }

    // Everything checks out (or errors have been served), set values:
    Object.assign(this.data, inputData)
   }

   getData() {
     return this.data
   }

   name() {
     return this.data.name
   }

   toggleVisibility(imageName) {
     if (this.data.imageList.includes(imageName)) {
       let idx = this.data.imageList.indexOf(imageName);
       if(idx != -1){ // list currently contains tag
         this.data.imageList.splice(idx, 1 );
       }
     }
     else {
       this.data.imageList.push(imageName)
     }
   }

   toggleGlobal() {
     this.data.global = !this.data.global
   }
 } // class: Filter
