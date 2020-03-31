'use strict';

export default class WaypointSegment{
  
  // Coordinates in px
  constructor(xPxCoordinate, yPxCoordinate){
    this._xPxCoordinate = Number(xPxCoordinate);
    this._yPxCoordinate = Number(yPxCoordinate);

    this._xCmCoordinate = null;
    this._yCmCoordinate = null;
  } 

  get xPxCoordinate() {
    return this._xPxCoordinate;
  }

  set xPxCoordinate(newXCoord) {
    this._xPxCoordinate = newXCoord;
  }

  get yPxCoordinate() {
    return this._yPxCoordinate;
  }

  set yPxCoordinate(newYCoord) {
    this._yPxCoordinate = newYCoord;
  }

  get xCmCoordinate() {
    return this._xCmCoordinate;
  }

  set xCmCoordinate(newXCoord) {
    this._xCmCoordinate = newXCoord;
  }

  get yCmCoordinate() {
    return this._yCmCoordinate;
  }
  
  set yCmCoordinate(newYCoord) {
    this._yCmCoordinate = newYCoord;
  }

 } 
