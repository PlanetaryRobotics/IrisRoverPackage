<template>
    <div id = "grid-container">
      <svg id = "grid" >
          <g id = "gridLines">
          </g>

          <g id ="gridContents">
            <g id ="routeModals"/>
            <RoverFan />
            <g id ="polarPlot" v-show="polarPlotEnabled"/>
            <Lander />
            <VisibleRoutes 
              :origin = {origin}
              :rover = {rover}
              :gridSquare = {gridSquare}
            />
            <Rover />
          </g>
      </svg>
    </div>
</template>

<script>

import VisibleRoutes from '@/components/Map/MapComponents/GridComponents/VisibleRoutes.vue';
import Lander from '@/components/Map/MapComponents/GridComponents/Lander.vue';
import Rover from '@/components/Map/MapComponents/GridComponents/Rover.vue';
import RoverFan from '@/components/Map/MapComponents/GridComponents/RoverFan.vue';

import $ from 'jquery';
import * as d3 from "d3";
import GridEventBus from '@/components/Map/GridEventBus.js';
import { mapGetters } from 'vuex';
import { plotNewSegment, 
         generateFirstSegmentVars, 
         generateAppendedSegmentVars, 
         calculateCmCoordinatesForSegment,
         getAbsoluteCoordinates } from '@/components/Map/Utility/SegmentPlotter.js';
import WaypointSegment from "@/data_classes/WaypointSegment.js";

export default {
  name: "Grid",
  components: {
    VisibleRoutes,
    Lander,
    Rover,
    RoverFan
  },
  data() {
    return {
      // Lander coords always = origin
      origin: {
        xPosPx: "",
        yPosPx: "",
      },
      rover: {
        angle: -30,
        xCmFromLander: 30,
        yCmFromLander: 30,
        xPosPx: "",
        yPosPx: ""
      },
      grid: {
        width: null,
        height: null,
        numSquares: 10,
      },
      gridSquare: {
        gridUnitPx: null,
        gridUnitCm: 30
      },
      polarPlot: {
        numBisections: 3,
        ringSeparationCm: 30,
        diameterCm: 1000, //100m
      },
      mouseCoords: null,
    }
  },
  computed: {
    ...mapGetters({
      routeList: 'routeList',
      polarPlotEnabled: 'polarPlotEnabled',
      isListeningForWaypoint: 'isListeningForWaypoint',
      removeCurrSegment: 'removeCurrSegment',

      // AddToRoute.vue
      appendedSegmentData: 'appendedSegmentData',
      appendedSegmentUpdate: 'appendedSegmentUpdate',
      removeAppendedSegment: 'removeAppendedSegment',
    }),
  },
  mounted() {
    this.initializeGridSize();
    this.drawGrid();
    this.setLander();
    this.setRover();
    this.drawPolar();

    GridEventBus.$on('WAYPOINT_FORM_UPDATE', (data) => {
      this.updateCurrWaypointSegment(data.xCm, data.yCm);
    })
  },
  watch: {
    polarPlotEnabled(isEnabled) {
      if (isEnabled) {
        d3.select("#gridLines")
           .selectAll("line")
           .attr("stroke-opacity", 0.5)
      } else {
        d3.select("#gridLines")
           .selectAll("line")
           .attr("stroke-opacity", 1)
      }
    },
    removeCurrSegment() {
      if (!d3.select("#NewRoute").empty()) {
         d3.select("#NewRoute").remove();
      }
    },
    appendedSegmentUpdate() {
      this.drawAppendedSegment();
    },
    removeAppendedSegment() {
      if (!d3.select("#Appended-Segment-Editing").empty()) {
        d3.select("#Appended-Segment-Editing").remove();
      }
    }
  },
  methods: {
    updateCurrWaypointSegment(xCm, yCm) {
      let currWaypointSegment = new WaypointSegment();
      currWaypointSegment.setCmCoordinates(xCm, yCm);

      let {angle, startX, startY, endX, endY} = generateFirstSegmentVars(currWaypointSegment, 
                                                              this.rover.xPosPx, 
                                                              this.rover.yPosPx,
                                                              this.origin.xPosPx,
                                                              this.origin.yPosPx,
                                                              this.gridSquare.gridUnitCm,
                                                              this.gridSquare.gridUnitPx,
                                                              );
      
      this.$store.commit("setCurrWaypointSegment", currWaypointSegment);

      let currRouteTransform;

      if (d3.select("#NewRoute").empty()) {
        currRouteTransform = 
          d3.select("#gridContents")
            .append('g')
            .lower()
            .attr("id", "NewRoute")
            .append('g')
            .attr("id", "NewRoute-Group")
            .append('g')
            .attr("id", "NewRoute-Segment0");

      } else {
        currRouteTransform = d3.select("#NewRoute")
                                .select("#NewRoute-Segment0");
        currRouteTransform.selectAll("*").remove();
      }

      plotNewSegment(currRouteTransform, "NewRoute", 0, angle, startX, startY, endX, endY, true);
    },
    gridClicked() {
      // isListening means form is set to waypoint
      if (this.isListeningForWaypoint) {

        let currWaypointSegment = new WaypointSegment();
        currWaypointSegment.setPxCoordinates(this.mouseCoords[0], this.mouseCoords[1]);

        let {angle, startX, startY, endX, endY} = generateFirstSegmentVars(currWaypointSegment, 
                                                              this.rover.xPosPx, 
                                                              this.rover.yPosPx,
                                                              this.origin.xPosPx,
                                                              this.origin.yPosPx,
                                                              this.gridSquare.gridUnitCm,
                                                              this.gridSquare.gridUnitPx,
                                                              );

        GridEventBus.$emit('WAYPOINT_GRID_UPDATE', {xCm: currWaypointSegment.xCmCoordinate, 
                                                    yCm: currWaypointSegment.yCmCoordinate});

        this.$store.commit("setCurrWaypointSegment", currWaypointSegment);

        let currRouteTransform;

        if (d3.select("#NewRoute").empty()) {
          currRouteTransform = 
            d3.select("#gridContents")
              .append('g')
              .lower()
              .attr("id", "NewRoute")
              .append('g')
              .attr("id", "NewRoute-Group")
              .append('g')
              .attr("id", "NewRoute-Segment0");

        } else {
          currRouteTransform = d3.select("#NewRoute")
                                  .select("#NewRoute-Segment0");
          currRouteTransform.selectAll("*").remove();
        }

        plotNewSegment(currRouteTransform, "NewRoute", 0, angle, startX, startY, endX, endY, true);
      } 
    },
    drawPolar() {
      let ringDistance = (this.polarPlot.ringSeparationCm/ this.gridSquare.gridUnitCm) * this.gridSquare.gridUnitPx;
      let radius = (this.polarPlot.diameterCm/ this.gridSquare.gridUnitCm) * this.gridSquare.gridUnitPx;
      let numRings = radius/ringDistance;

      let diameter = radius*2;
      let domain = 10;
      let pos = getAbsoluteCoordinates(d3.select("#roverGroup"));

      let svg = d3.select("#polarPlot")
                  .attr("transform", `translate(${pos.x-radius}, ${pos.y-radius}) rotate(${this.rover.angle}, ${radius}, ${radius})`)
                  .append("svg")
                  .attr("width", diameter)
                  .attr("height", diameter);

      let radialScale = d3.scaleLinear()
                          .domain([0,domain])
                          .range([0,radius]);
      
      // Getting evenly spaced arr of ints within
      // domain 
      let ticks = [];
      for (let i = 1; i <= numRings; i++) {
        ticks.push((domain/numRings) * i);
      }

      ticks.forEach(t =>
            svg.append("circle")
            .attr("cx", radius)
            .attr("cy", radius)
            .attr("fill", "none")
            .attr("stroke", "#A56DFF")
            .attr("r", radialScale(t))
        );

      // Plotting lines
      function angleToCoordinate(angle, value){
        let x = Math.cos(angle) * radialScale(value);
        let y = Math.sin(angle) * radialScale(value);
        return {"x": radius + x, "y": radius - y};
      }

      let numLines = this.polarPlot.numBisections * 2;

      for (let i = 1; i <= numLines; i++) {
        // let ft_name = features[i];
        let angle = (Math.PI / 2) + ((2 * Math.PI * i) / numLines);
        let line_coordinate = angleToCoordinate(angle, 10);
        //let label_coordinate = angleToCoordinate(angle, 10.5);

        //draw axis line
        svg.append("line")
        .attr("x1", radius)
        .attr("y1", radius)
        .attr("x2", line_coordinate.x)
        .attr("y2", line_coordinate.y)
        .attr("stroke","#A56DFF");

        //draw axis label
        // svg.append("text")
        // .attr("x", label_coordinate.x)
        // .attr("y", label_coordinate.y)
        // .text(ft_name);
      }
    },
    drawAppendedSegment() {
      let route = this.appendedSegmentData.route;
      let segment = this.appendedSegmentData.segment;
      let lastSegmentIdx = route.segmentList.length-1;

      let {computedAngle, startX, startY, endX, endY} = generateAppendedSegmentVars(route, 
                                                                                    segment, 
                                                                                    lastSegmentIdx, 
                                                                                    this.gridSquare.gridUnitCm, 
                                                                                    this.gridSquare.gridUnitPx,
                                                                                    this.rover.xPosPx,
                                                                                    this.rover.yPosPx,
                                                                                    this.origin.xPosPx,
                                                                                    this.origin.yPosPx,
                                                                                    this.rover.xCmFromLander,
                                                                                    this.rover.yCmFromLander);
      
      let transform;

      if (d3.select("#Appended-Segment-Editing").empty()) {
        transform = d3.select("#"+route.routeName+"-Group")
                      .append('g')
                      .attr("id", "Appended-Segment-Editing");
      } else {
        transform = d3.select("#Appended-Segment-Editing");
        transform.selectAll("*").remove();
      }

      plotNewSegment(transform, route.routeName, lastSegmentIdx+1, computedAngle, startX, startY, endX, endY, true);

      // Calculate coords for relatives
      if (segment.constructor.name === "RelativeSegment") {

        let roverTrans = this.convertCmToPx(this.rover.xCmFromLander, this.rover.yCmFromLander);
        calculateCmCoordinatesForSegment(segment, 
                                            "Appended-Segment-Editing", 
                                            lastSegmentIdx+1, 
                                            this.rover.xPosPx - roverTrans.xPx, //Subtract by rover trans
                                            this.rover.yPosPx - roverTrans.yPx, 
                                            this.gridSquare.gridUnitCm, 
                                            this.gridSquare.gridUnitPx);
      }
    },

    /**
     * Performs necessary computations to plot the line and point
     * for the route depending on the tab it is on.
     * 
     * Will draw the route if there is at least distance defined.
     * 
     * @param {object} segment      Segment object
    */
    drawCurrRoute(segment) {

      // Only draw if there is AT LEAST distance param inputted
      if (segment.constructor.name === "RelativeSegment" &&
         (segment.distance === "")) {
           return;
      }

      if (segment.constructor.name === "AbsoluteSegment" &&
         (segment.xCoordinate === 0 && segment.yCoordinate === 0)) {
           return;
      }

      let {angle, startX, startY, endX, endY} = generateFirstSegmentVars(segment, 
                                                              this.rover.xPosPx, 
                                                              this.rover.yPosPx,
                                                              this.origin.xPosPx,
                                                              this.origin.yPosPx,
                                                              this.gridSquare.gridUnitCm,
                                                              this.gridSquare.gridUnitPx,
                                                              );
      
      let currRouteTransform;

      if (d3.select("#NewRoute").empty()) {
        currRouteTransform = 
          d3.select("#gridContents")
            .append('g')
            .lower()
            .attr("id", "NewRoute")
            .append('g')
            .attr("id", "NewRoute-Group")
            .append('g')
            .attr("id", "NewRoute-Segment0");

      } else {
        currRouteTransform = d3.select("#NewRoute")
                                .select("#NewRoute-Segment0");
        currRouteTransform.selectAll("*").remove();
      }

      plotNewSegment(currRouteTransform, "NewRoute", 0, angle, startX, startY, endX, endY, true);

      // Calculate coords for relatives
      if (segment.constructor.name === "RelativeSegment") {

        let roverTrans = this.convertCmToPx(this.rover.xCmFromLander, this.rover.yCmFromLander);
        calculateCmCoordinatesForSegment(segment, 
                                            "NewRoute", 
                                            0, 
                                            this.rover.xPosPx - roverTrans.xPx, //Subtract by rover trans
                                            this.rover.yPosPx - roverTrans.yPx, 
                                            this.gridSquare.gridUnitCm, 
                                            this.gridSquare.gridUnitPx);
      }
    },

    /**
     * Initializes the grid size by using client height/width
     * of parent container.
    */
    initializeGridSize() {
      var parent = $("#grid-container")[0];

      this.grid.width = parent.clientWidth;
      this.grid.height = parent.clientHeight;
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
      var width = this.grid.width;
      var height = this.grid.height;

      // Set # of px per grid unit
      this.gridSquare.gridUnitPx = width/this.grid.numSquares;

      // Set origin coord to be center of the grid
      this.origin.xPosPx = ((Math.ceil(this.grid.numSquares/2))*this.gridSquare.gridUnitPx);
      this.origin.yPosPx = ((Math.ceil(this.grid.numSquares/2))*this.gridSquare.gridUnitPx);

      var svg = d3.select("#grid")
                  .attr("width","100%")
                  .attr("height","100%")
                  .attr("viewBox", `0 0 ${width} ${height}`)
                  .attr("preserveAspectRatio", "none")

      var boxG = svg.select("#gridLines");

      var numBoxes = this.grid.numSquares;
      var arr = d3.range(0,numBoxes+1);
      var boxSize = width/numBoxes;
      var boxWidth = width/this.grid.numSquares;
      var boxHeight = height/this.grid.numSquares;

      var boxEnter = boxG.selectAll("line")
                         .data(arr)
                         .enter();
      // Verticals
      boxEnter.append("line")
              .attr("x1", function (d){return d*boxWidth})
              .attr("x2", function (d){return d*boxWidth;})
              .attr("y1", -boxHeight)
              .attr("y2", height + boxWidth)
              .style("stroke", "white");

      // Horizontals
      boxEnter.append("line")
              .attr("x1", -boxWidth)
              .attr("x2", width + boxWidth)
              .attr("y1", function (d){return d*boxWidth})
              .attr("y2", function (d){return d*boxWidth})
              .style("stroke", "white");

      var zoom = d3.zoom()
                  .scaleExtent([1, 3])
                  .on("zoom", zoomed);

      function zoomed() {
        
        let x = d3.event.transform.x;
        let y = d3.event.transform.y;
        let k = d3.event.transform.k;

        boxG.attr("transform", "translate(" + x%(boxSize*k)+","+y%(boxSize*k) + ")scale(" + k + ")");
        
        d3.select("#gridContents")
          .attr("transform", "translate(" + x + "," + y + ")scale(" + k + ")");
      }

      // Watch the mouse click inside grid and convert the coords
      let that = this;
      svg.on("click", function() {
        var xy = d3.mouse(this);
        
        var transform = d3.zoomTransform(svg.node());
        var xy1 = transform.invert(xy);

        that.mouseCoords = xy1;
        that.gridClicked();
      })

      svg.call(zoom)
         // To center move 1/2 box up
         //.call(zoom.transform, d3.zoomIdentity.translate(0, -boxWidth/2).scale(1))
    },

    /**
     * Sets position of the lander to be at center of grid.
    */
    setLander() {
      let svg = $('#lander')[0];

      let left = this.origin.xPosPx - svg.getBBox().width/2;
      let top = this.origin.yPosPx - svg.getBBox().height/2;

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
      let yPos = originY + coord.yPx - roverHeight/2;
      let xPos = originX + coord.xPx - roverWidth/2;

      // Save rover's position (without rover centering adjustment)
      this.rover.xPosPx = xPos + d3.select("#rover").node().getBBox().width/2;
      this.rover.yPosPx = yPos + d3.select("#rover").node().getBBox().height/2;

      // Set rover coords
      d3.select("#roverGroup")
        .attr("transform", `translate(${xPos}, ${yPos}) rotate(${this.rover.angle}, ${roverWidth/2}, ${roverHeight/2})`);

      // Get fan coordinates and adjust so bottom middle is at coord
      yPos = originY + coord.yPx - d3.select("#roverFanSVG").node().getBBox().height;
      xPos = originX + coord.xPx - d3.select("#roverFanSVG").node().getBBox().width/2;

      // Set fan coords
      d3.select("#roverFanGroup")
        .attr("transform", `translate(${xPos}, ${yPos}) rotate(${this.rover.angle}, ${d3.select("#roverFanSVG").node().getBBox().width/2}, ${d3.select("#roverFanSVG").node().getBBox().height})`);
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

      return {xPx: xPx, yPx: yPx};
    }
  }
}

</script>

<style lang="scss" scoped>

@import '@/styles/_colors.scss';
@import '@/styles/_functional.scss';

#lander, #rover {
  position: absolute;
  left: 0px; 
  top: 0px;
}

#grid-container {
  position: relative;
}

</style>
