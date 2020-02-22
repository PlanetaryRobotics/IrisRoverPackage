'use strict';
import ImageData from "@/data_classes/ImageData.js";
import Tag from "@/data_classes/Tag.js";
import testImage from "@/components/POI/testImage.jpg";

export default class POICard{
  
  constructor(inputData){

    // Default 
    this.data = 
    {
      importanceLevel: null,
      category: null,
      thumbnail: {     // TODO: temp fix for demo purposes - figure out how to pull 
                       //       img from DB in store 
        url: testImage
      },
      //thumbnail: null,
      width: null,
      height: null,
      sizeUnit: null,
      tagList: [],
      creator: null,
      description: null,
      images: [],
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
    } else {
      this.data.images.push(inputData.thumbnail);
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
  }

  getData() {
    return this.data;
  }

  addImage(image) {
    if (!(image instanceof ImageData)){
      console.error("Image is not of type ImageData. Image not added to POICard.");
      console.log(typeof image);
    } else {
      this.data.images.push(image);
    }
  }
  
  static get CATEGORY_COLORS() {
    return {ATTRACTION: "#21DF84", OBSTACLE: "#FF2F46", SHADOW: "#FF2F46"};
  }

  static get OBJECT_SIZE_UNITS() {
    return ["CM"];
  }

 } 
