'use strict';
import States from './WaypointStates.js';

export default class WaypointSegment{
  
    constructor(gridSquareSize){
        this.gridSquareSize = gridSquareSize; // Contains the size of a grid square in px and cm

        this._xCmCoordinate = null;
        this._yCmCoordinate = null;

        this._roverAngle = null;
        this._state = States.Waypoint.UNVISITED;
    }

    setPxCoordinates(xPx, yPx) {
        this.xPxCoordinate = xPx;
        this.yPxCoordinate = yPx;
    }

    setCmCoordinates(xCm, yCm) {
        this.xCmCoordinate = xCm;
        this.yCmCoordinate = yCm;
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
        return this._xCmCoordinate * this.gridSquareSize.gridUnitPx / this.gridSquareSize.gridUnitCm;
    }

    set xPxCoordinate(newXCoord) {
        this._xCmCoordinate = newXCoord * this.gridSquareSize.gridUnitCm / this.gridSquareSize.gridUnitPx;
    }

    get yPxCoordinate() {
        return -this._yCmCoordinate * this.gridSquareSize.gridUnitPx / this.gridSquareSize.gridUnitCm;
    }

    set yPxCoordinate(newYCoord) {
        this._yCmCoordinate = -newYCoord * this.gridSquareSize.gridUnitCm / this.gridSquareSize.gridUnitPx;
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
