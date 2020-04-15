//import WaypointSegment from '@/data_classes/WaypointSegment.js';
import RouteList from '@/data_classes/RouteList.js';

export default {
    state: {
        addRouteButton: {
          clicked: false,
          hovered: false
        },

        planSegmentButton: {
          clicked: false,
          hovered: false
        },

        cancelSegmentButton: {
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

        // CREATE FIRST SEG TO A ROUTE
        isListeningForWaypoint: false,
        removeCurrSegment: 0,
        currWaypointSegment: null,
        editingRoute: null,

        // ROUTELIST
        routeList: new RouteList(),
        routeListUpdate: 0, // For watching the visibility toggles on routes.vue
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

      // --- Waypoint related
      isListeningForWaypoint: state => {
        return state.isListeningForWaypoint;
      },

      currWaypointSegment: state => {
        return state.currWaypointSegment;
      },

      removeCurrSegment: state => {
        return state.removeCurrSegment;
      },

      editingRoute: state => {
        return state.editingRoute;
      },
    
    },
    mutations: {
      createEmptyRoute(state) {
        state.routeList.addEmptyRoute();
      },

      saveSegment(state, {route, segment}) {
        route.addToSegmentList(segment);
        this.commit("triggerRouteListUpdate");
        this.commit("triggerCurrSegmentRemoval");
      },

      togglePolarPlotButton(state) {
        state.PolarPlotSVG.enabled = !state.PolarPlotSVG.enabled;
      },

      triggerRouteListUpdate(state) {
        state.routeListUpdate += 1;
      },

      triggerCurrSegmentRemoval(state) {
        state.currSegment = null;
        state.removeCurrSegment += 1;
      },

      // -- Waypoint related
      setIsListeningForWaypoint(state, isListening) {
        state.isListeningForWaypoint = isListening;
      },

      setCurrWaypointSegment(state, segment) {
        state.currWaypointSegment = segment;
      },

      setEditingRoute(state, route) {
        state.editingRoute = route;
      },

      // saveSegment(state, {route, segment}) {
      //   let idx = state.routeList.findIndex(r => r.routeName === route.routeName);
      //   let target = state.routeList[idx];

      //   target.addToSegmentList(segment);

      //   // Update state
      //   state.routeList[idx] = target;
      //   state.appendedSegmentData = {
      //     segment: null,
      //     route: null,
      //   };
      // }

    }
};
