'use strict';
import Route from "@/data_classes/Route.js";

export default class RouteList{
  constructor(){
    this._routes = [];
    this._routeNames = new Set();
    this._untitledNum = 1;
  } 

  get routes() {
    return this._routes;
  }

  renameRouteName(oldName, newName) {
    if (this.hasRouteName(newName)) {
      return false;
    }

    let targetRoute = this._routes.find(r => r.routeName === oldName);
    targetRoute.routeName = newName;

    this._routeNames.delete(oldName);
    this._routeNames.add(newName);

    return true;
  }

  hasRouteName(name) {
    return this._routeNames.has(name);
  }

  isListEmpty() {
    return this._routes.length === 0;
  }

  addRoute(route) {
    if (!(route instanceof Route)) {
      console.error("Input is not of type Route.");
      return;
    }

    if (this.hasRouteName(route.routeName)) {
      console.error("Route name already exists.");
      console.log(route.routename);
    }

    this._routes.push(route);
    this._routeNames.add(route.routeName);
  }

  addEmptyRoute() {

    let name = "Untitled" + this._untitledNum;

    while (this._routeNames.has(name)) {
      this._untitledNum++;
      name = "Untitled" + this._untitledNum;
    }

    let r = new Route(name, true, null);

    this._routes.push(r);
    this._routeNames.add(r.routeName);

    this._untitledNum++;
  }

  delete(route) {
    let idx = this._routes.indexOf(route);
    this._routes.splice(idx, 1);
    this._routeNames.delete(route.routeName);
  }
} 
