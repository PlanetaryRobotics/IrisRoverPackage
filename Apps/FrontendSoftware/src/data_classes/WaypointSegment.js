'use strict';

export default class WaypointSegment{
  
  // Coordinates in px
  constructor(){
    this._xPxCoordinate = null;
    this._yPxCoordinate = null;

    this._xCmCoordinate = null;
    this._yCmCoordinate = null;
  } 

  setPxCoordinates(xPx, yPx) {
    this._xPxCoordinate = xPx;
    this._yPxCoordinate = yPx;
  }

  setCmCoordinates(xCm, yCm) {
    this._xCmCoordinate = xCm;
    this._yCmCoordinate = yCm;
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
