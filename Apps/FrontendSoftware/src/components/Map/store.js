import Route from '@/data_classes/Route.js';
import AbsoluteSegment from '@/data_classes/AbsoluteSegment.js';
import RelativeSegment from '@/data_classes/RelativeSegment.js';
// import WaypointSegment from '@/data_classes/WaypointSegment.js';
import RouteList from '@/data_classes/RouteList.js';

export default {
    state: {
        addRouteButton: {
          clicked: false,
          hovered: false
        },

        // POLAR PLOT BUTTON:
        PolarPlotSVG: {
          clicked: true,
          hovered: false,
          enabled: false,
        },

        // RIGHT NAV RADIO BUTTONS:
        RouteAndFeaturesRadioSVG: {
          clicked: true,
          hovered: false
        },
        RouteManagerRadioSVG: {
          clicked: false,
          hovered: false
        },

        // CANCEL & PLAN ROUTE BUTTONS
        // (in add/edit route tab)
        planRouteButton: {
          clicked: false,
          hovered: false
        },
        cancelRouteButton: {
          clicked: false,
          hovered: false
        },

        // CREATE ROUTE
        currSegment: null,
        currSegmentUpdate: 0,
        removeCurrSegment: 0,

        currWaypointSegment: null,
        
        // ADD TO ROUTE
        appendedSegmentData: {
          segment: null,
          route: null,
        },
        appendedSegmentUpdate: 0,
        removeAppendedSegment: 0,

        // ROUTELIST
        // routeList: [new Route("WaypointRoute", true, new WaypointSegment(600, 300)),
        //             new Route("WaypointRoute2", true, new WaypointSegment(600, 600)),
        //             ],
        routeList: new RouteList(),
        routeListUpdate: 0, // For watching the visibility toggles on routes.vue

        isListeningForWaypoint: false,
    },
    getters: {
      polarPlotEnabled: state => {
        return state.PolarPlotSVG.enabled;
      },

      routeList: state => {
        return state.routeList;
      },

      routeListIsEmpty: state => {
        return state.routeList.isListEmpty();
      },
      
      routeListUpdate: state => {
        return state.routeListUpdate;
      },

      // CreateRoute.vue
      currSegment: state => {
        return state.currSegment;
      },

      currSegmentUpdate: state => {
        return state.currSegmentUpdate;
      },

      removeCurrSegment: state => {
        return state.removeCurrSegment;
      },

      // --- Waypoint related
      isListeningForWaypoint: state => {
        return state.isListeningForWaypoint;
      },

      currWaypointSegment: state => {
        return state.currWaypointSegment;
      },

      // AddToRoute.vue
      appendedSegmentData: state => {
        return state.appendedSegmentData;
      },

      appendedSegmentUpdate: state => {
        return state.appendedSegmentUpdate;
      },

      removeAppendedSegment: state => {
        return state.removeAppendedSegment;
      }
    
    },
    mutations: {
      togglePolarPlotButton(state) {
        state.PolarPlotSVG.enabled = !state.PolarPlotSVG.enabled;
      },

      triggerRouteListUpdate(state) {
        state.routeListUpdate += 1;
      },

      //////////////////////////////////
      // CreateRoute.vue
      //////////////////////////////////

      triggerCurrSegmentRemoval(state) {
        state.currSegment = null;
        state.removeCurrSegment += 1;
      },

      saveCurrRoute(state, {routeName, routeType}) {
        let segment;
        if (routeType === "waypoint") {
          segment = state.currWaypointSegment;
          state.currWaypointSegment = null;
        } else {
          segment = state.currSegment;
          state.currSegment = null;
        }

        let route = new Route(routeName, true, segment);
        state.routeList.addRoute(route);
        this.commit("triggerCurrSegmentRemoval");
      },

      updateCurrSegment(state, {distance, angle, xCoord, yCoord, segmentType}) { 
        if (segmentType === "relative") {
          state.currSegment = new RelativeSegment(distance, angle);
        } else if (segmentType === "absolute") {
          state.currSegment = new AbsoluteSegment(xCoord, yCoord);
        }
        state.currSegmentUpdate += 1;
      },

      // -- Waypoint related
      setIsListeningForWaypoint(state, isListening) {
        state.isListeningForWaypoint = isListening;
      },

      setCurrWaypointSegment(state, segment) {
        state.currWaypointSegment = segment;
      },

      //////////////////////////////////
      // AddToRoute.vue
      //////////////////////////////////

      triggerAppendedSegmentRemoval(state) {
        state.appendedSegmentData = {
          segment: null,
          route: null,
        };
        state.removeAppendedSegment += 1;
      },

      updateAppendedSegment(state, {route, routeType, distance, angle, xCoord, yCoord}) {
        state.appendedSegmentData.route = route;
        if (routeType === "relative") {
          state.appendedSegmentData.segment = new RelativeSegment(distance, angle);
        } else {
          state.appendedSegmentData.segment = new AbsoluteSegment(xCoord, yCoord);
        }
        state.appendedSegmentUpdate += 1;
      },

      saveSegment(state, {route, segment}) {
        let idx = state.routeList.findIndex(r => r.routeName === route.routeName);
        let target = state.routeList[idx];

        target.addToSegmentList(segment);

        // Update state
        state.routeList[idx] = target;
        state.appendedSegmentData = {
          segment: null,
          route: null,
        };
      }

    }
};
