import Route from '@/data_classes/Route.js';
import AbsoluteSegment from '@/data_classes/AbsoluteSegment.js';
import RelativeSegment from '@/data_classes/RelativeSegment.js';

export default {
    state: {
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

        // ADD TO ROUTE
        appendedSegmentData: {
          segment: null,
          route: null,
        },
        appendedSegmentUpdate: 0,
        removeAppendedSegment: 0,

        // ROUTELIST
        routeList: [new Route("RelativeRoute", true, new RelativeSegment(30, 30)),
                    new Route("AbsoluteRoute", true, new AbsoluteSegment(60, -30))],
        routeListUpdate: 0, // For watching the visibility toggles on routes.vue
    },
    getters: {
      polarPlotEnabled: state => {
        return state.PolarPlotSVG.enabled;
      },

      routeList: state => {
        return state.routeList;
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

      saveCurrRoute(state, routeName) {
        let route = new Route(routeName, true, state.currSegment);
        state.routeList.push(route);
        state.currSegment = null;
        this.commit("triggerCurrSegmentRemoval");
      },

      updateCurrSegment(state, {distance, angle, xCoord, yCoord, routeType}) { 
        if (routeType === "relative") {
          state.currSegment = new RelativeSegment(distance, angle);
        } else {
          state.currSegment = new AbsoluteSegment(xCoord, yCoord);
        }
        state.currSegmentUpdate += 1;
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