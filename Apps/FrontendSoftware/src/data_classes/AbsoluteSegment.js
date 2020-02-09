'use strict';

export default class AbsoluteSegment{
  
  // Coordinates in cm 
  constructor(xCoordinate, yCoordinate){
    // Empty check is to handle blank input on form
    if (xCoordinate === "") {
      this._xCoordinate = 0;
    }
    else {
      this._xCoordinate = Number(xCoordinate);
    }
    
    if (yCoordinate === "") {
      this._yCoordinate = 0;
    } else {
      this._yCoordinate = Number(yCoordinate);
    }

    this._angle = null;
  } 

  get xCoordinate() {
    return this._xCoordinate;
  }

  set xCoordinate(newXCoord) {
    this._xCoordinate = newXCoord;
  }

  get yCoordinate() {
    return this._yCoordinate;
  }

  set yCoordinate(newYCoord) {
   this._yCoordinate = newYCoord;
  }

  get angle() {
    return this._angle;
  }

  set angle(angle) {
    this._angle = angle;
  }
 } 
