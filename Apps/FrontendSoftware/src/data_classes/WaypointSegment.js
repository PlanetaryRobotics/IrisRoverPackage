'use strict';
import States from './WaypointStates.js'

export default class WaypointSegment{
  
  constructor(){
    this._xPxCoordinate = null;
    this._yPxCoordinate = null;

    this._xCmCoordinate = null;
    this._yCmCoordinate = null;

    this._roverAngle = null;
    this._state = States.Waypoint.UNVISITED;
  } 

  setPxCoordinates(xPx, yPx) {
    this._xPxCoordinate = xPx;
    this._yPxCoordinate = yPx;
  }

  setCmCoordinates(xCm, yCm) {
    this._xCmCoordinate = xCm;
    this._yCmCoordinate = yCm;
  }

  setAsVisited() {
    this._state = States.Waypoint.VISITED;
  }

  get state() {
    return this._state;
  }

  get roverAngle() {
    return this._roverAngle;
  }

  set roverAngle(angle) {
    this._roverAngle = angle;
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
