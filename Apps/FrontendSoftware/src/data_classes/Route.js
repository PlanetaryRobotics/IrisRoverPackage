'use strict';
import { v1 as uuidv1 } from 'uuid';
import * as d3 from "d3";

export default class Route{
   constructor(routeName, isVisible, firstSegment){
      this._routeName = routeName;
      this._isVisible = isVisible;
      this._uuid = uuidv1();

      if (!firstSegment) {
        this._segmentList = [];
      } else {
        this._segmentList = [firstSegment];
      }
   } 

   get uuid() {
      return this._uuid;
   }
   
   get routeName() {
     return this._routeName;
   }

   set routeName(newName) {
    if (newName !== ""){
      this._routeName = newName;
    }
   }

   get isVisible() {
     return this._isVisible;
   }

   set isVisible(bool) {
    this._isVisible = bool;
   }

   get segmentList() {
     return this._segmentList;
   }

   addToSegmentList(newSegment) {
     if (newSegment.constructor.name === "WaypointSegment") {
      this._segmentList.push(newSegment);
      this._uuid = uuidv1(); // So vue v-for will know to update
     }
     else {
        throw new Error("Trying to add an object that is not a Segment to SegmentList of Route.");
     }
   }

   deleteSegment(segment) {
     let idx = this._segmentList.indexOf(segment);
     if (idx === -1) {
       throw new Error("Segment does not exist.");
     }
     this.segmentList.splice(idx, 1);
     this._uuid = uuidv1(); // So vue v-for will know to update
   }

   setVisibility() {
      let visibility = "none";
      if (this._isVisible) {
        visibility = "block";
      }

      d3.select("#" + this._routeName + "-Group")
        .style("display", visibility);
   }
 } 
