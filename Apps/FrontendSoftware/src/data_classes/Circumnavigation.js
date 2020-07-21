'use strict';

export default class Circumnavigation{
  
  constructor(startAngle, endAngle, photos, radius, isClockwise, POICard, POIPosition, waypoints = []){
    this._startAngle = startAngle;
    this._endAngle = endAngle;
    this._numPhotos = photos;
    this._radius = radius;
    this._isClockwise = isClockwise;
    this._POICard = POICard;
    this._POIPosition = POIPosition;

    this._waypoints = waypoints;
  } 

  get startAngle() {
    return this._startAngle;
  }

  set startAngle(angle) {
    this._startAngle = angle;
  }

  get endAngle() {
    return this._endAngle;
  }

  set endAngle(angle) {
    this._endAngle = angle;
  }

  get numPhotos() {
    return this._numPhotos;
  }

  set numPhotos(numPhotos) {
    this._numPhotos = numPhotos;
  }

  get radius() {
    return this._radius;
  }

  set radius(radius) {
    this._radius = radius;
  }

  get isClockwise() {
    return this._isClockwise;
  }

  set isClockwise(isClockwise) {
    this._isClockwise = isClockwise;
  }

  get POICard() {
    return this._POICard;
  }

  set POICard(card) {
    this._POICard = card;
  }

  get POIPosition() {
    return this._POIPosition;
  }

  set POIPosition(position) {
    this._POIPosition = position;
  }

  get waypoints() {
    return this._waypoints;
  }

  set waypoints(newWaypoints) {
    this._waypoints = newWaypoints;
  }

 } 
