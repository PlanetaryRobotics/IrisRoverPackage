'use strict';
import POICard from "@/data_classes/POICard.js";

export default class POIList{
  
  constructor(POICardsArray){

    this._list = [];
    for (let card of POICardsArray) {
      if (!(card instanceof POICard)) {
        console.error("Card is not of type ImageData.");
        console.log(card);
      } else {
        this._list.push(card);
      }
    }
  } 

  get list() {
    return this._list;
  }

  addPOI(POICard) {
    if (!(POICard instanceof POICard)) {
      console.error("Card is not of type ImageData.");
      console.log(POICard);
    } else {
      this._list.push(POICard);
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
    if (!order) {
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
