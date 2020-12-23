import LocalizationTestList from '@/components/Map/Test/LocalizationTestList.js';
import RouteList from '@/data_classes/RouteList.js';
import { getReorganizedList } from "@/components/Map/Utility/LocalizationDataPostProcessor.js";

export default {
  state: {
    // Metadata about the grid
    gridMetadata: {
      xPosPx: "",
      yPosPx: "",
      gridUnitPx: null,
      gridUnitCm: 30,
      width: "",
      height: "",
      numSquares: 10,
    },
    // Metadata about drawn rover:
    roverMetadata: {
      angle: 0,
      xCmFromLander: 0,
      yCmFromLander: 0,
      xPosPx: "",
      yPosPx: "",
    },

    // BUTTON STATES:
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
    isListeningForNewWaypoint: false,
    removeCurrSegment: 0,
    currWaypointSegment: null,
    editingRoute: null,

    // EDITING A SEG
    isListeningForEditWaypoint: false,
    editingSegmentInfo: {
      route: null,
      segmentIdx: null,
    },

    // CIRCUMNAV
    isListeningForPOIClick: false,
    targetPOI: {
      POICard: null,
      positionPx: null,
    },
    currCircumnav: null, // Temp circumnav when adding
    editingCircumnav: null, // Temp cirumnav when editing

    // ROUTELIST
    routeList: new RouteList(),

    // LOCALIZATION DATA
    localizationData: new LocalizationTestList(),
    oldList: null,
    framePoints: [],

    // COMMAND DATA
    // TODO: this will eventually have to pull from CLI store 
    commandData: []
  },

  getters: {
    // get grid meta
    getGridMetadata: state => {
      return state.gridMetadata;
    },
    // --- DB Sync
    commandData: state => {
      return state.commandData;
    },

    localizationData: state => {
      let newList = state.localizationData.getList();

      // No list initialized yet
      if (!state.oldList) {
        state.oldList = [...newList];
        return state.oldList;
      }

      // This happens when we get new data with SAME lookupID, so length of list is the same
      if (state.oldList.length === newList.length) {
        state.oldList = [...newList];
        return state.oldList;
      }

      // Check if the new obj is a frame, if so, add to framePoints
      if (newList[newList.length - 1].data.frame.n > 0 && newList[newList.length - 1].data.frame.len > 0) {
        state.framePoints.push(newList[newList.length - 1]);
      }

      // Reorder the list
      let adjustedList = getReorganizedList(state.oldList, newList);

      // Do a copy otherwise saving mem addr
      state.oldList = [...adjustedList];

      return state.oldList;
    },

    framePoints: state => {
      return state.framePoints;
    },

    // --- Polar plot
    polarPlotEnabled: state => {
      return state.PolarPlotSVG.enabled;
    },

    // --- Routelist
    routeList: state => {
      return state.routeList;
    },

    routeListIsEmpty: state => {
      return state.routeList.isListEmpty();
    },

    // --- Add Waypoint related
    isListeningForNewWaypoint: state => {
      return state.isListeningForNewWaypoint;
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

    // --- Edit Waypoint related
    isListeningForEditWaypoint: state => {
      return state.isListeningForEditWaypoint;
    },

    editingSegmentInfo: state => {
      return state.editingSegmentInfo;
    },

    // --- Add Circumnav related
    isListeningForPOIClick: state => {
      return state.isListeningForPOIClick;
    },

    targetPOI: state => {
      return state.targetPOI;
    },

    currCircumnav: state => {
      return state.currCircumnav;
    },

    // --- Edit Circumnav related
    editingCircumnav: state => {
      return state.editingCircumnav;
    },
  },
  mutations: {
    // update functions for update grid meta data
    updateGridMetadata(state, data) {
      Object.assign(state.gridMetadata, data);
    },
    // update function for update rover meta data
    updateRoverMetadata(state, data) {
      Object.assign(state.roverMetadata, data);
    },

    createEmptyRoute(state) {
      state.routeList.addEmptyRoute();
    },

    saveSegment(state, { route, segment }) {
      route.addToSegmentList(segment);
      state.currWaypointSegment = null;
    },

    saveCircumnav(state, route) {
      route.addToSegmentList(state.currCircumnav);
      state.currCircumnav = null;
    },

    togglePolarPlotButton(state) {
      state.PolarPlotSVG.enabled = !state.PolarPlotSVG.enabled;
    },

    // -- Adding waypoint
    setIsListeningForNewWaypoint(state, isListening) {
      state.isListeningForNewWaypoint = isListening;
    },

    setCurrWaypointSegment(state, segment) {
      state.currWaypointSegment = segment;
    },

    setEditingRoute(state, route) {
      state.editingRoute = route;
    },

    // -- Editing waypoint
    setIsListeningForEditWaypoint(state, isListening) {
      state.isListeningForEditWaypoint = isListening;
    },

    setEditingSegmentInfo(state, { route, segmentIdx }) {
      state.editingSegmentInfo = {
        route: route,
        segmentIdx: segmentIdx
      }
    },

    // -- Delete waypoint
    deleteWaypoint(state, { route, segment }) {
      route.deleteSegment(segment);
    },

    // -- Adding circumnav
    setIsListeningForPOIClick(state, isListening) {
      state.isListeningForPOIClick = isListening;
    },

    setTargetPOI(state, { POICard, positionPx }) {
      state.targetPOI = Object.assign({}, { POICard: POICard, positionPx: positionPx });
    },

    setCurrCircumnav(state, circumnav) {
      state.currCircumnav = circumnav;
    },

    // -- Editing circumnav
    setEditingCircumnav(state, circumnav) {
      state.editingCircumnav = circumnav;
    },

    // -- CLI testing (remove later)
    addCommand(state, data) {
      state.commandData.push(data);
    }
  }
};
