'use strict';
/*
  * Defines Standard Form for Data that Should be Contained by a Tag.
  * This allows for easy and consistent formatting of Tags.
  *
  * An example of creating a filter is:
  * Filter({
      name: "TagName",
      initialImage: "CR_M01_0001",
      tagsList: ['list', 'of', 'existing', 'tag', 'names']
    })
  *
  * To access filter data use:
  * .getImages() // Returns list of images assigned to tag
  * .getName()   // Returns tag name
  *
  * To add or remove images use:
  * .addImage('imageName')      // Adds image to tagsList
  * .removeImage('imageName')   // Removes image from tagsList
  *
  * Author: John Walker Moosbrugger, CMU
  * Created: 4/14/2019
  * Last Update: 11/1/2019, Colombo
 */
import { v1 as uuidv1 } from 'uuid';

export default class Tag{
  constructor(inputData){
    /*
      Constructs a new Tag from the given data.
      The format of the data is a object with keys for the name of the tag and the name of the first image associated with the tag.
    */
    this.data = {
      id: "" + uuidv1(),
      name: "",
      images: [],
      description: "",
      //TODO: Pull user names/objects from DB
      modificationHistory: [
        {"user": "Caitlin Coyiuto", "time": Tag.formatTime(new Date(new Date().setDate(new Date().getDate()-1)))}, 
        {"user": "Caitlin Coyiuto", "time": Tag.formatTime(new Date(Date.now()))},
      ]
    }

    // Validates name is formatted as a string that is not longer than 30 characters and is not empty
    if ( inputData.name === "" || typeof inputData.name != "string" ) {
      console.error("Invalid tag name in Tag constructor. Name is empty or not a string.");
      console.log(inputData.name);
    }
    else if ( inputData.name.length > 30 ) {
      console.error("Invalid tag name in Tag constructor. Name is too long.")
      console.log(inputData.name)
    }
    else if ( inputData.tagNames.includes(inputData.name) ) {
      console.error("Tag name already used, cannot have duplicate tag names.")
      console.log(inputData.name)
    }

    // Saves description if it exists
    if (inputData.description) {
      this.data.description = inputData.description;
    }

    // Validates image exists
    if(inputData.initialImage){
      if (!(this.validatateImage(inputData.initialImage))) { // if image is not valid
        console.error("Invalid image name in Tag constructor. Image Does not exist")
        console.log(inputData.initialImage)
      }
      this.data.images.push(inputData.initialImage)
    }

    // Everything checks out (or errors have been served), set values:
    this.data.name = inputData.name
  }

  validatateImage(imageName) {
    // Validates the initial image exists
    // This is dependant on where and how we store our master list of images @Connor Colombo
    if (imageName === "") {
      return false // TODO FIXME
    }
    else {
      return true
    }
  }

  getImages() { // Returns the list of images associated with the tag
    return this.data.images
  }

  getName() { // Returns the name of the tag
    return this.data.name
  }

  toggleImage(imageName) {
    if (this.data.images.includes(imageName)) {
      this.removeImage(imageName)
    }
    else {
      this.addImage(imageName)
    }
  }

  addImage(imageName) { // Takes an input of an image name, validates it, and adds it to the images list
    if (this.validatateImage(imageName)) {
      this.data.images.push(imageName)
    }
    else {
      console.error("Invalid image name in Tag removeImage method. Image Does not exist or is already included in this tag")
      console.log(imageName)
    }
  }

  removeImage(imageName) { // takes an input of an image name and removes it from the images list.
    let idx = this.data.images.indexOf(imageName);
    if(idx != -1){ // list currently contains tag
      this.data.images.splice(idx, 1);
    }
  }

  static formatTime(obj) {
    let year = obj.getFullYear();
    let month = obj.getUTCMonth() + 1; 
    let day = obj.getUTCDate();
    let hours = obj.getHours();
    let min = obj.getMinutes();

    function addZero(x) {
      if (x.toString().length === 1) {
        return "0" + x;
      }
      return x;
    }

    return addZero(month) + "-" + addZero(day) + "-" + year + " " + addZero(hours) + ":" + addZero(min);
  }
}
