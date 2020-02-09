'use strict';

export default class RelativeSegment{
   constructor(distance, angle){
      // Empty check is to handle blank input on form
      if (distance === "") {
        this._distance = 0;
      } else {
        this._distance = distance;
      }
      this._angle = angle;
      this._xCoordinate = null;
      this._yCoordinate = null;
   } 

   get distance() {
     return this._distance;
   }

   set distance(distance) {
    if (distance !== "") {
      this._distance = distance;
    }
   }

   get angle() {
     return this._angle;
   }
   
   set angle(angle) {
     if (angle !== "") {
      this._angle = angle;
     }
   }

   set xCoordinate(xCm) {
    this._xCoordinate = xCm;
   }

   get xCoordinate() {
     return this._xCoordinate;
   }

   set yCoordinate(yCm) {
     this._yCoordinate = yCm;
   }

   get yCoordinate() {
     return this._yCoordinate;
   }

 } 
