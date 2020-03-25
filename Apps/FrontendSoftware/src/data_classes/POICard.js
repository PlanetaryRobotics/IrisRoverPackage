'use strict';
import ImageData from "@/data_classes/ImageData.js";
import Tag from "@/data_classes/Tag.js";

export default class POICard{
  
  constructor(inputData){

    // Default 
    this.data = 
    {
      number: 0,
      importanceLevel: null,
      category: null,
      thumbnail: null,
      width: null,
      height: null,
      depth: null,
      sizeUnit: null,
      tagList: [],
      creator: null,
      description: null,
      images: [],
      createTimeObject: null,
      modificationHistory: [
        {"user": "Caitlin Coyiuto", "time": POICard.formatTime(new Date(Date.now()))} //TODO: Pull user name from DB
      ]
    }
    
    this.validationChecks(inputData);
    Object.assign(this.data, inputData);
  } 

  validationChecks(inputData) {

    // Validation checks on importanceLevel
    if (typeof inputData.importanceLevel !== "number" || inputData.importanceLevel === "") {
      console.error("Importance level is not a number or is empty.");
      console.log(inputData.importanceLevel);
    }
    else if (inputData.importanceLevel < 1 || inputData.importanceLevel > 3) {
      console.log("Importance level must be between 1 and 3.");
      console.log(inputData.importanceLevel);
    }
    
    // Validation checks on category
    if (typeof inputData.category !== "string" || inputData.category === "") {
      console.error("Category is not a string or not defined.");
      console.log(inputData.category);
    }
    else if (!Object.keys(POICard.CATEGORY_COLORS).includes(inputData.category)) {
      console.error("Not a valid category. Must be one of: " + Object.keys(POICard.CATEGORY_COLORS));
      console.log(inputData.category);
    }
    
    // Validation check on ImageinputData
    if (!(inputData.thumbnail instanceof ImageData)) {
      console.error("Thumbnail is not of type ImageData.");
    }  else {
      inputData.images.unshift(inputData.thumbnail);
      inputData.createTimeObject = inputData.thumbnail.timeObject;
    }

    // Validation check on width
    if (typeof inputData.width !== "number" || inputData.width === "") {
      console.error("Width is not a number or not defined.");
      console.log(inputData.width);
    }

    // Validation check on height
    if (typeof inputData.height !== "number" || inputData.height === "") {
      console.error("Height is not a number or not defined.");
      console.log(inputData.height);
    }

    // Validation check on depth
    if (typeof inputData.depth !== "number" || inputData.depth === "") {
      console.error("Depth is not a number or not defined.");
      console.log(inputData.depth);
    }

    // Validation check on sizeUnit
    if (typeof inputData.sizeUnit !== "string" || inputData.sizeUnit === "") {
      console.error("Size unit is not a string or not defined.");
      console.log(inputData.sizeUnit);
    }
    else if (!POICard.OBJECT_SIZE_UNITS.includes(inputData.sizeUnit)) {
      console.error("Size unit is not valid.");
      console.log(inputData.sizeUnit);
    }

    // Validation check on tagList
    if (!Array.isArray(inputData.tagList)) {
      console.error("tagList must be an array.");
      console.log(inputData.tagList);
    }
    else if (inputData.tagList.length === 0) {
      console.error("tagList is empty.");
    } else {
      for (let tag of inputData.tagList) {
        if (!(tag instanceof Tag)) {
          console.error("Object in TagList is not of type Tag.");
          console.log(tag);
        }
      }
    }

    // Validation check on creator
    // TODO: this will probably pass in a specific object for user - check w/ Connor
    if (inputData.creator === "") {
      console.error("Creator is not defined.");
    }

    // Validation check on description
    if (typeof inputData.description !== "string" || inputData.description === "") {
      console.error("Description is not of type string of is empty.");
      console.log(inputData.description);
    }

    // Validation check on images
    for (let image of inputData.images) {
      if (!(image instanceof ImageData)) {
        console.error("Image is not of type ImageData");
        console.log(image);
      } 
    }
  }

  getData() {
    return this.data;
  }

  getName() {
    let str = "";
    if (this.data.category === "ATTRACTION") {
      str += "ATTR-";
    } else if (this.data.category === "OBSTACLE") {
      str += "OBST-";
    } else {
      str += "SHDW-";
    }

    return str += this.data.number;
  }

  getColor() {
    let category = this.data.category;
    return POICard.CATEGORY_COLORS[category];
  }

  addImage(image) {
    if (!(image instanceof ImageData)){
      console.error("Image is not of type ImageData. Image not added to POICard.");
      console.log(typeof image);
    } else {
      this.data.images.push(image);
    }
  }

  addToModificationHistory() {
    let user = "Caitlin Coyiuto"; // TODO: pull from DB for this
    let time = POICard.formatTime(new Date(Date.now()));

    this.data.modificationHistory.push({user: user, time: time});
  }

  set category(newCategory) {
    if (!Object.keys(POICard.CATEGORY_COLORS).includes(newCategory)) {
      console.error("New category for POI is not valid.");
      console.log(newCategory);
    }
    this.data.category = newCategory;
  }

  set number(number) {
    if (this.isNumber(number)) {
      this.data.number = number;
    }
  }

  set width(width) {
    if (this.isNumber(width)) {
      this.data.width = width;
    }
  }

  set height(height) {
    if (this.isNumber(height)) {
      this.data.height = height;
    }
  }

  set depth(depth) {
    if (this.isNumber(depth)) {
      this.data.depth = depth;
    }
  }

  set description(newDescription) {
    this.data.description = newDescription;
  }

  set importanceLevel(newLevel) {
    this.data.importanceLevel = newLevel;
  }

  isNumber(value) {
    if (isNaN(value)) {
      console.error("Value is not an number");
      console.log(value);
      return false;
    }
    return true;
  }

  removeTag(tag) {
    let tagList = this.data.tagList;
    this.data.tagList = tagList.filter(item => item !== tag);
  }

  removeImage(image) {
    let images = this.data.images;
    this.data.images = images.filter(item => item !== image);
  }

  static get CATEGORY_COLORS() {
    return {ATTRACTION: "#21DF84", OBSTACLE: "#FF2F46", SHADOW: "#FF2F46"};
  }

  static get OBJECT_SIZE_UNITS() {
    return ["CM"];
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
