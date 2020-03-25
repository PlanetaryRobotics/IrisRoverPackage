'use strict';
import POICard from "@/data_classes/POICard.js";

export default class POIList{
  
  constructor(POICardsArray){

    this._list = {ATTRACTION: [], OBSTACLE: [], SHADOW: []};
    this._POIByTagList = new Map();  // { TAG OBJ : [POICARD, POICARD ... ] }

    for (let card of POICardsArray) {
      this.addPOI(card);
    }
  } 

  get list() {
    let result = [];
    
    Object.keys(this._list).forEach(key => {
      result.push(...this._list[key]);
    })

    return result;
  }

  get POIByTagList() {
    return this._POIByTagList;
  }

  nextNumForCategory(category) {
    return this._list[category].length + 1;
  }

  updatePOICategory(POI, newCategory) {
    let newNum = this.nextNumForCategory(newCategory);

    // Remove POI from old category list
    this.deletePOI(POI);

    // Set new num and category, then add to list
    POI.number = newNum;
    POI.category = newCategory;
    this.addPOI(POI);
  }

  addPOI(card) {
    if (!(card instanceof POICard)) {
      console.error("Card is not of type ImageData.");
      console.log(card);
    } else {
      let category = card.getData().category;
      let num = this._list[category].length + 1;
      card.number = num;

      this._list[category].push(card);
      this.addToPOITagList(card);
    }
  }

  deletePOI(card) {
    if (!(card instanceof POICard)) {
      console.error("Card is not of type ImageData.");
      console.log(card);
    } else {
      let category = card.getData().category;
      this._list[category] = this._list[category].filter(item => item !== card);
      this.removeFromPOITagList(card);
    }
  }

  addToPOITagList(card) {
    let tagsInCard = card.getData().tagList;

    for (let tag of tagsInCard) {
      if (!this._POIByTagList.has(tag)){
        this._POIByTagList.set(tag, [card]);
      } else {
        let arr = this._POIByTagList.get(tag);
        this._POIByTagList.set(tag, [...arr, card]);
      }
    }
  }

  removeFromPOITagList(card) {
    let tagsInCard = card.getData().tagList;

    for (let tag of tagsInCard) {
      let array = this._POIByTagList.get(tag);
      let index = array.indexOf(card);
      if (index !== -1) {
        array.splice(index, 1);
        this._POIByTagList.set(tag, array);
      }
    }
  }

  static filterBy(filter, POIList) {
    if (!filter) {
      return POIList;
    }

    let VALIDFILTERS = ["ATTRACTION", "OBSTACLE", "SHADOW"];
    let res = [];

    if (!VALIDFILTERS.includes(filter)) {
      console.error("Filter " + filter + " is not valid.");
    } else {
      POIList.forEach(card => {
        if (card.getData().category === filter) {
          res.push(card);
        }
      })
    }

    return res;
  }

  static orderBy(order, POIList) {

    if (!order || order === "null") {
      return POIList;
    }

    let VALIDORDERS = ["IMPORTANCE", "TIME ADDED"];

    if (!VALIDORDERS.includes(order)) {
      console.error("Order " + order + " is not valid.");
    } else {
      if (order === "IMPORTANCE") {
        // Force immutability w/ slice()
        return POIList.slice().sort((a, b) => b.getData()["importanceLevel"] - a.getData()["importanceLevel"]);
      } else {
         // Force immutability w/ slice()
        return POIList.slice().sort((a, b) => b.getData()["createTimeObject"]- a.getData()["createTimeObject"]);
      }
    }
  }
 } 
