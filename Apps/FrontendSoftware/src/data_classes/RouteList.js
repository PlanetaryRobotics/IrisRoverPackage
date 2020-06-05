'use strict';
import Route from "@/data_classes/Route.js";
import { v1 as uuidv1 } from 'uuid';

export default class RouteList{
  constructor(){
    this._routes = [];
    this._routeNames = new Set();
    this._uniqueIDS = new Set();
    this._untitledNum = 1;
  } 

  get routes() {
    return this._routes;
  }

  renameRouteName(oldName, newName) {
    if (this.hasRouteName(newName)) {
      return;
    }

    let targetRoute = this._routes.find(r => r.routeName === oldName);
    targetRoute.routeName = newName;

    this._routeNames.delete(oldName);
    this._routeNames.add(newName);
  }

  hasRouteName(name) {
    return this._routeNames.has(name);
  }

  updateUUID(route) {
    let newUUID = uuidv1();

    while (this._uniqueIDS.has(newUUID)) {
      newUUID = uuidv1();
    }

    this._uniqueIDS.delete(route.uuid);

    route.uuid = newUUID;
    this._uniqueIDS.add(newUUID);
  }

  isListEmpty() {
    return this._routes.length === 0;
  }

  addEmptyRoute() {
    let name = "Untitled" + this._untitledNum;

    while (this._routeNames.has(name)) {
      this._untitledNum++;
      name = "Untitled" + this._untitledNum;
    }

    let r = new Route(name, true, null);

    while (this._uniqueIDS.has(r.uuid)) {
      r.uuid = uuidv1();
    }
    this._uniqueIDS.add(r.uuid);

    this._routes.push(r);
    this._routeNames.add(r.routeName);

    this._untitledNum++;
  }

  delete(route) {
    let idx = this._routes.indexOf(route);
    this._routes.splice(idx, 1);
    this._routeNames.delete(route.routeName);
    this._uniqueIDS.delete(route.uuid);
  }
} 
