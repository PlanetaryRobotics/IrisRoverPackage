'use strict';
import Route from "@/data_classes/Route.js";

export default class RouteList{
  constructor(){
    this._routes = [];
    this._routeNames = new Set();
  } 

  get routes() {
    return this._routes;
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
} 
