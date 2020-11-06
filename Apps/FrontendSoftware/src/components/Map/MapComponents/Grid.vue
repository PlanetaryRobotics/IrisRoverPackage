<template>
  <div id="grid-container" ref="container">
    <!-- <resize-observer @notify="handleResize" /> -->
    <h1 style="color: white">{{ grid.width }}</h1>
    <!-- <svg id="grid" v-bind:viewBox.camel="svgSize.viewbox"> -->
    <svg id="grid">
      <!-- GRID LINES -->
      <g id="gridLines"></g>

      <!-- GRID CONTENT -->
      <g id="gridContents">
        <!-- POI OBJECTS -->
        <g id="POIObjects" v-if="show.POIObjects">
          <POIObject
            v-for="POI in $store.state.POI.POIList.list"
            :POI="POI"
            :key="POI.uuid"
            :positionPx="computePOIPxPosition(POI)"
          />
        </g>

        <!-- MODALS -->
        <g id="routeModals" />

        <!-- ROVER FAN -->
        <RoverFan />

        <!-- POLAR PLOT -->
        <g id="polarPlot" v-show="polarPlotEnabled" />

        <!-- LANDER -->
        <Lander />

        <!-- SAVED ROUTES -->
        <VisibleRoutes :origin="{ origin }" :gridSquare="{ gridSquare }" />

        <!-- ROVER'S TRAILS -->
        <LocalizedTrail
          :origin="{ origin }"
          :rover="{ rover }"
          :gridSquare="{ gridSquare }"
        />
        <TargetTrail
          :origin="{ origin }"
          :rover="{ rover }"
          :gridSquare="{ gridSquare }"
        />

        <!-- ROVER -->
        <Rover />
      </g>
    </svg>
  </div>
</template>

<script>
import VisibleRoutes from "@/components/Map/MapComponents/GridComponents/VisibleRoutes.vue";
import Lander from "@/components/Map/MapComponents/GridComponents/Lander.vue";
import Rover from "@/components/Map/MapComponents/GridComponents/Rover.vue";
import RoverFan from "@/components/Map/MapComponents/GridComponents/RoverFan.vue";
import POIObject from "@/components/Map/MapComponents/GridComponents/POIObject.vue";
import LocalizedTrail from "@/components/Map/MapComponents/GridComponents/LocalizedTrail.vue";
import TargetTrail from "@/components/Map/MapComponents/GridComponents/TargetTrail.vue";

import $ from "jquery";
import * as d3 from "d3";
import GridEventBus from "@/components/Map/GridEventBus.js";
import { mapGetters } from "vuex";
import {
  plotNewSegment,
  generateFirstSegmentVars,
  getAbsoluteCoordinates,
  updateExistingSegment,
  calculateCmToPxCoords,
  calculatePxToCmCoords,
} from "@/components/Map/Utility/SegmentPlotter.js";
import {
  plotNewCircumnav,
  replaceExistingCircumnav,
} from "@/components/Map/Utility/CircumnavPlotter.js";
import WaypointSegment from "@/data_classes/WaypointSegment.js";
// var throttle = require("lodash/throttle");

export default {
  name: "Grid",
  components: {
    VisibleRoutes,
    Lander,
    Rover,
    RoverFan,
    POIObject,
    LocalizedTrail,
    TargetTrail,
  },
  data() {
    return {
      // Lander coords always = origin
      origin: {
        xPosPx: "",
        yPosPx: "",
      },
      // Init values
      rover: {
        angle: 0,
        xCmFromLander: 0,
        yCmFromLander: 0,
        xPosPx: "",
        yPosPx: "",
      },
      svgSize: {
        viewbox: null,
      },
      grid: {
        width: null,
        height: null,
        numSquares: 10,
      },
      gridSquare: {
        gridUnitPx: null,
        gridUnitCm: 30,
      },
      polarPlot: {
        numBisections: 3,
        ringSeparationCm: 30,
        diameterCm: 1000, //100m
      },
      mouseCoords: null,
      show: {
        POIObjects: false,
      },
      resizeObserver: {},
    };
  },
  computed: {
    ...mapGetters([
      "routeList",
      "polarPlotEnabled",
      "isListeningForNewWaypoint",
      "isListeningForEditWaypoint",
      "editingRoute",
      "editingSegmentInfo",
      "localizationData",
      "currWaypointSegment",
      "currCircumnav",
    ]),
  },
  mounted() {
    // Initialize main objects on grid
    this.initializeGridSize();
    this.drawGrid();
    this.setLander();
    this.setRover();
    this.drawPolar();
    // Now let POIs render since grid constant computations are done
    this.show.POIObjects = true;

    // Event listener when a new segment needs to update
    GridEventBus.$on("ADD_SEG_FORM_UPDATE", (data) => {
      this.plotNewSegmentPreview(data.xCm, data.yCm, data.angle);
    });

    // Event listener when an existing segment needs to update
    GridEventBus.$on("EDIT_SEG_FORM_UPDATE", (data) => {
      let coords = {
        xCm: data.xCm,
        yCm: data.yCm,
      };

      updateExistingSegment(
        data.route,
        data.segmentIndex,
        coords,
        data.roverAngle,
        this.origin.xPosPx,
        this.origin.yPosPx,
        this.gridSquare.gridUnitCm,
        this.gridSquare.gridUnitPx
      );
    });

    // Event listener to calculate + set the PX coords for the segment
    GridEventBus.$on("COMPUTE_SEG_PX_COORDS", (segment) => {
      let { xPx, yPx } = calculateCmToPxCoords(
        segment.xCmCoordinate,
        segment.yCmCoordinate,
        this.origin.xPosPx,
        this.origin.yPosPx,
        this.gridSquare.gridUnitCm,
        this.gridSquare.gridUnitPx
      );
      segment.setPxCoordinates(xPx, yPx);
    });

    // Event listener when a new circumnav segment set needs to update
    GridEventBus.$on("ADD_CIRCUM_FORM_UPDATE", (data) => {
      let circumnav = plotNewCircumnav(
        data,
        this.editingRoute,
        this.origin.xPosPx,
        this.origin.yPosPx,
        this.gridSquare.gridUnitCm,
        this.gridSquare.gridUnitPx
      );
      this.$store.commit("setCurrCircumnav", circumnav);
    });

    // Event listener when an existing circumnav needs to be updated
    GridEventBus.$on("EDIT_CIRCUM_FORM_UPDATE", (payload) => {
      let { route, segmentIndex, data } = payload;
      let circumnav = replaceExistingCircumnav(
        data,
        route,
        segmentIndex,
        this.origin.xPosPx,
        this.origin.yPosPx,
        this.gridSquare.gridUnitCm,
        this.gridSquare.gridUnitPx
      );
      this.$store.commit("setEditingCircumnav", circumnav);
    });
  },
  beforeDestroy() {
    GridEventBus.$off("ADD_SEG_FORM_UPDATE");
    GridEventBus.$off("EDIT_SEG_FORM_UPDATE");
    GridEventBus.$off("COMPUTE_SEG_PX_COORDS");
    GridEventBus.$off("ADD_CIRCUM_FORM_UPDATE");
    GridEventBus.$off("EDIT_CIRCUM_FORM_UPDATE");
  },
  watch: {
    polarPlotEnabled(isEnabled) {
      if (isEnabled) {
        d3.select("#gridLines").selectAll("line").attr("stroke-opacity", 0.5);
      } else {
        d3.select("#gridLines").selectAll("line").attr("stroke-opacity", 1);
      }
    },
    currWaypointSegment(state) {
      if (state === null) {
        if (!d3.select("#NewRoute").empty()) {
          d3.select("#NewRoute").remove();
        }
      }
    },
    localizationData(newData) {
      let idx = newData.length - 1; // Using last in list to move rover

      // Set the rover's new position
      this.rover.xCmFromLander = newData[idx].data.position[0];
      this.rover.yCmFromLander = newData[idx].data.position[1];
      this.rover.angle = newData[idx].data.position[2];

      this.setRover();
    },
    currCircumnav(state) {
      if (state === null) {
        if (!d3.select("#NewCircum").empty()) {
          d3.select("#NewCircum").remove();
        }
      }
    },
  },
  methods: {
    plotNewSegmentPreview(xCm, yCm, roverAngle) {
      let currWaypointSegment = new WaypointSegment();
      currWaypointSegment.setCmCoordinates(xCm, yCm);

      if (roverAngle && roverAngle !== "") {
        currWaypointSegment.roverAngle = roverAngle;
      }

      let { angle, startX, startY, endX, endY } = generateFirstSegmentVars(
        this.editingRoute,
        currWaypointSegment,
        this.origin.xPosPx,
        this.origin.yPosPx,
        this.gridSquare.gridUnitCm,
        this.gridSquare.gridUnitPx
      );

      this.$store.commit("setCurrWaypointSegment", currWaypointSegment);

      let currRouteTransform;

      if (d3.select("#NewRoute").empty()) {
        currRouteTransform = d3
          .select("#gridContents")
          .append("g")
          .lower()
          .attr("id", "NewRoute")
          .append("g")
          .attr("id", "NewRoute-Group")
          .append("g")
          .attr("id", "NewRoute-Segment0");
      } else {
        currRouteTransform = d3
          .select("#NewRoute")
          .select("#NewRoute-Segment0");
        currRouteTransform.selectAll("*").remove();
      }

      plotNewSegment(
        currRouteTransform,
        "NewRoute",
        0,
        angle,
        startX,
        startY,
        endX,
        endY,
        true,
        roverAngle,
        currWaypointSegment.state
      );
    },
    gridClicked() {
      // Add form is open, and listening for any clicks on grid
      if (this.isListeningForNewWaypoint) {
        let currWaypointSegment = new WaypointSegment();
        currWaypointSegment.setPxCoordinates(
          this.mouseCoords[0],
          this.mouseCoords[1]
        );

        let { angle, startX, startY, endX, endY } = generateFirstSegmentVars(
          this.editingRoute,
          currWaypointSegment,
          this.origin.xPosPx,
          this.origin.yPosPx,
          this.gridSquare.gridUnitCm,
          this.gridSquare.gridUnitPx
        );

        GridEventBus.$emit("WAYPOINT_GRID_UPDATE", {
          xCm: currWaypointSegment.xCmCoordinate,
          yCm: currWaypointSegment.yCmCoordinate,
        });

        this.$store.commit("setCurrWaypointSegment", currWaypointSegment);

        let currRouteTransform;

        if (d3.select("#NewRoute").empty()) {
          currRouteTransform = d3
            .select("#gridContents")
            .append("g")
            .lower()
            .attr("id", "NewRoute")
            .append("g")
            .attr("id", "NewRoute-Group")
            .append("g")
            .attr("id", "NewRoute-Segment0");
        } else {
          currRouteTransform = d3
            .select("#NewRoute")
            .select("#NewRoute-Segment0");
          currRouteTransform.selectAll("*").remove();
        }

        plotNewSegment(
          currRouteTransform,
          "NewRoute",
          0,
          angle,
          startX,
          startY,
          endX,
          endY,
          true,
          null,
          currWaypointSegment.state
        );
      }
      // An edit form is open, and listening for any clicks
      else if (this.isListeningForEditWaypoint) {
        let coords = {
          xPx: this.mouseCoords[0],
          yPx: this.mouseCoords[1],
        };

        // Roverangle is same as current
        let roverAngle = this.editingSegmentInfo.route.segmentList[
          this.editingSegmentInfo.segmentIdx
        ].roverAngle;

        updateExistingSegment(
          this.editingSegmentInfo.route,
          this.editingSegmentInfo.segmentIdx,
          coords,
          roverAngle,
          this.origin.xPosPx,
          this.origin.yPosPx,
          this.gridSquare.gridUnitCm,
          this.gridSquare.gridUnitPx
        );

        let { xCm, yCm } = calculatePxToCmCoords(
          coords.xPx,
          coords.yPx,
          this.origin.xPosPx,
          this.origin.yPosPx,
          this.gridSquare.gridUnitCm,
          this.gridSquare.gridUnitPx
        );

        GridEventBus.$emit("WAYPOINT_GRID_UPDATE", { xCm: xCm, yCm: yCm });
      }
    },
    drawPolar() {
      let ringDistance =
        (this.polarPlot.ringSeparationCm / this.gridSquare.gridUnitCm) *
        this.gridSquare.gridUnitPx;
      let radius =
        (this.polarPlot.diameterCm / this.gridSquare.gridUnitCm) *
        this.gridSquare.gridUnitPx;
      let numRings = radius / ringDistance;

      let diameter = radius * 2;
      let domain = 10;
      let pos = getAbsoluteCoordinates(d3.select("#roverGroup"));

      let svg = d3
        .select("#polarPlot")
        .attr(
          "transform",
          `translate(${pos.x - radius}, ${pos.y - radius}) rotate(${
            this.rover.angle
          }, ${radius}, ${radius})`
        )
        .append("svg")
        .attr("width", diameter)
        .attr("height", diameter);

      let radialScale = d3.scaleLinear().domain([0, domain]).range([0, radius]);

      // Getting evenly spaced arr of ints within
      // domain
      let ticks = [];
      for (let i = 1; i <= numRings; i++) {
        ticks.push((domain / numRings) * i);
      }

      ticks.forEach((t) =>
        svg
          .append("circle")
          .attr("cx", radius)
          .attr("cy", radius)
          .attr("fill", "none")
          .attr("stroke", "#A56DFF")
          .attr("r", radialScale(t))
      );

      // Plotting lines
      function angleToCoordinate(angle, value) {
        let x = Math.cos(angle) * radialScale(value);
        let y = Math.sin(angle) * radialScale(value);
        return { x: radius + x, y: radius - y };
      }

      let numLines = this.polarPlot.numBisections * 2;

      for (let i = 1; i <= numLines; i++) {
        // let ft_name = features[i];
        let angle = Math.PI / 2 + (2 * Math.PI * i) / numLines;
        let line_coordinate = angleToCoordinate(angle, 10);
        //let label_coordinate = angleToCoordinate(angle, 10.5);

        //draw axis line
        svg
          .append("line")
          .attr("x1", radius)
          .attr("y1", radius)
          .attr("x2", line_coordinate.x)
          .attr("y2", line_coordinate.y)
          .attr("stroke", "#A56DFF");

        //draw axis label
        // svg.append("text")
        // .attr("x", label_coordinate.x)
        // .attr("y", label_coordinate.y)
        // .text(ft_name);
      }
    },
    /**
     * Function to handle update grid size when window size changes
     **/
    updateGrid() {
      // select and remove old grid
      const svg = d3.select("#grid");
      svg.select("#gridLines").selectAll("line").remove();
      // redraw grid and rover
      this.drawGrid();
      this.setLander();
      this.setRover();
      this.drawPolar();
      // TODO update exisiting routes
    },
    /**
     * Function to handle grid update when window resize
     */
    handleResize() {
      // update grid width and height state
      this.grid.width = this.$refs.container.clientWidth;
      this.grid.height = this.$refs.container.clientHeight;
      // update the grid size
      this.updateGrid();
    },
    /**
     * Initializes the grid size by using client height/width
     * of parent container.
     */
    initializeGridSize() {
      var parent = $("#grid-container")[0];
      this.grid.width = parent.clientWidth;
      this.grid.height = parent.clientHeight;
      // select grid container to watch window resizes
      const gridContainer = document.querySelector("#grid-container");
      // set inital state of the observer start status
      let observerStarted = true;
      // resize observer to watch window size changes
      const resizeObserver = new ResizeObserver(() => {
        // check if resize observer already started to avoid repeatative upadte
        if (observerStarted) {
          observerStarted = false;
          return;
        }
        // disconnect the existing observer to avoid fire inbetween update
        resizeObserver.disconnect();
        // update the grid
        this.handleResize();
        observerStarted = true;
        requestAnimationFrame(() => resizeObserver.observe(gridContainer));
      });
      // start obeserve window changes
      resizeObserver.observe(gridContainer);
    },
    /**
     * Draws the grid given what is stored in data.
     * Will let grid have pan/zoom functionality while maintaining
     * positions of all objects on grid.
     *
     * Additionally computes center coordinates in pixel units.
     * Computes the gridUnixPx value.
     *
     * Refactored from:
     * https://stackoverflow.com/questions/31698668/d3-js-how-can-i-make-an-infinite-background-grid
     */
    drawGrid() {
      const width = this.grid.width;
      const height = this.grid.height;

      // Set # of px per grid unit
      this.gridSquare.gridUnitPx = width / this.grid.numSquares;

      // Set origin coord to be center of the grid
      this.origin.xPosPx =
        Math.ceil(this.grid.numSquares / 2) * this.gridSquare.gridUnitPx;
      this.origin.yPosPx =
        Math.ceil(this.grid.numSquares / 2) * this.gridSquare.gridUnitPx;

      const svg = d3
        .select("#grid")
        .attr("width", "100%")
        .attr("height", "100%")
        .attr("viewBox", `0 0 ${width} ${height}`)
        .attr("preserveAspectRatio", "xMinYMin");

      const boxG = svg.select("#gridLines");

      const numBoxes = this.grid.numSquares;
      const boxSize = width / numBoxes;
      const boxWidth = width / this.grid.numSquares;
      const boxHeight = boxWidth;
      // set the range to be the the boxes match the longest side at init
      const arr = d3.range(
        0,
        width > height
          ? parseInt(width / boxWidth)
          : parseInt(height / boxWidth)
      );

      const boxEnter = boxG.selectAll("line").data(arr).enter();
      // Verticals
      boxEnter
        .append("line")
        .attr("x1", function (d) {
          return d * boxWidth;
        })
        .attr("x2", function (d) {
          return d * boxWidth;
        })
        .attr("y1", -boxHeight)
        .attr("y2", height + boxWidth)
        .style("stroke", "white");

      // Horizontals
      boxEnter
        .append("line")
        .attr("x1", -boxWidth)
        .attr("x2", width + boxWidth)
        .attr("y1", function (d) {
          return d * boxWidth;
        })
        .attr("y2", function (d) {
          return d * boxWidth;
        })
        .style("stroke", "white");
      const zoom = d3.zoom().scaleExtent([1, 3]).on("zoom", zoomed);

      function zoomed() {
        let x = d3.event.transform.x;
        let y = d3.event.transform.y;
        let k = d3.event.transform.k;

        boxG.attr(
          "transform",
          "translate(" +
            (x % (boxSize * k)) +
            "," +
            (y % (boxSize * k)) +
            ")scale(" +
            k +
            ")"
        );

        d3.select("#gridContents").attr(
          "transform",
          "translate(" + x + "," + y + ")scale(" + k + ")"
        );
      }

      // Watch the mouse click inside grid and convert the coords
      let that = this;
      svg.on("click", function () {
        const xy = d3.mouse(this);

        const transform = d3.zoomTransform(svg.node());
        const xy1 = transform.invert(xy);

        that.mouseCoords = xy1;
        that.gridClicked();
      });

      svg.call(zoom);
      // To center move 1/2 box up
      //.call(zoom.transform, d3.zoomIdentity.translate(0, -boxWidth/2).scale(1))
    },
    /**
     * Sets position of the lander to be at center of grid.
     */
    setLander() {
      let svg = $("#lander")[0];

      let left = this.origin.xPosPx - svg.getBBox().width / 2;
      let top = this.origin.yPosPx - svg.getBBox().height / 2;

      $("#landerGroup").attr("transform", `translate(${left}, ${top})`);
    },

    /**
     * Sets position of the lander given its cm coordinates
     * defined in the data().
     */
    setRover() {
      let originY = this.origin.yPosPx;
      let originX = this.origin.xPosPx;

      let yCm = this.rover.yCmFromLander;
      let xCm = this.rover.xCmFromLander;

      let coord = this.convertCmToPx(xCm, yCm);

      let roverWidth = d3.select("#rover").node().getBBox().width;
      let roverHeight = d3.select("#rover").node().getBBox().height;

      // These are coordinates w/ adjustment so centered at rover
      let yPos = originY + coord.yPx - roverHeight / 2;
      let xPos = originX + coord.xPx - roverWidth / 2;

      // Save rover's position (without rover centering adjustment)
      this.rover.xPosPx = xPos + d3.select("#rover").node().getBBox().width / 2;
      this.rover.yPosPx =
        yPos + d3.select("#rover").node().getBBox().height / 2;

      // Set rover coords
      d3.select("#roverGroup").attr(
        "transform",
        `translate(${xPos}, ${yPos}) rotate(${this.rover.angle}, ${
          roverWidth / 2
        }, ${roverHeight / 2})`
      );

      // Get fan coordinates and adjust so bottom middle is at coord
      yPos =
        originY + coord.yPx - d3.select("#roverFanSVG").node().getBBox().height;
      xPos =
        originX +
        coord.xPx -
        d3.select("#roverFanSVG").node().getBBox().width / 2;

      // Set fan coords
      d3.select("#roverFanGroup").attr(
        "transform",
        `translate(${xPos}, ${yPos}) rotate(${this.rover.angle}, ${
          d3.select("#roverFanSVG").node().getBBox().width / 2
        }, ${d3.select("#roverFanSVG").node().getBBox().height})`
      );
    },

    computePOIPxPosition(POI) {
      let coords = this.convertCmToPx(
        POI.getData().location[0],
        POI.getData().location[1]
      );
      let x = this.origin.xPosPx + coords.xPx;
      let y = this.origin.yPosPx + coords.yPx;

      return { xPx: x, yPx: y };
    },

    /**
     * Converts a coord in cm units to pixel units
     *
     * @param {number} xCm      Route id
     * @param {number} yCm      Starting x coord of line
     *
     * @return {object}         {xPx: xPx, yPx: yPx}
     */
    convertCmToPx(xCm, yCm) {
      let yPx = (yCm / this.gridSquare.gridUnitCm) * this.gridSquare.gridUnitPx;
      let xPx = (xCm / this.gridSquare.gridUnitCm) * this.gridSquare.gridUnitPx;

      // Invert sign since pos value means negative translation
      // (moving up) from origin
      yPx *= -1;

      return { xPx: xPx, yPx: yPx };
    },
  },
};
</script>

<style lang="scss" scoped>
@import "@/styles/_colors.scss";
@import "@/styles/_functional.scss";

#lander,
#rover {
  position: absolute;
  left: 0px;
  top: 0px;
}

#grid-container {
  position: relative;
}
</style>
