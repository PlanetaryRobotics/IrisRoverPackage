'use strict';

export default class Route{
   constructor(routeName, isVisible, firstSegment){
      this._routeName = routeName;
      this._isVisible = isVisible;
      this._segmentList = [firstSegment];
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
     }
    //  if (newSegment.constructor.name === "RelativeSegment" ||
    //      newSegment.constructor.name === "AbsoluteSegment") {
    //     this._segmentList.push(newSegment);
     else {
        throw new Error("Trying to add an object that is not a Segment to SegmentList of Route.");
     }
   }
 } 
