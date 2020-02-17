'use strict';

export default class WaypointSegment{
  
  // Coordinates in cm 
  constructor(xCoordinate, yCoordinate){
    this._xCoordinate = Number(xCoordinate);
    this._yCoordinate = Number(yCoordinate);
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
 } 
