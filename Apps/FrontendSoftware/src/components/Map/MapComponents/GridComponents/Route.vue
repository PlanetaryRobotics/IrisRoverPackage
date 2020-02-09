<template>
    <g :id = "routeName">
    </g>
</template>

<script>

import $ from 'jquery';
import * as d3 from "d3";
import { mapGetters } from 'vuex';
import { plotNewSegment, 
         generateFirstSegmentVars, 
         generateAppendedSegmentVars, 
         calculateRelativeSegmentCoordinates } from '../../Utility/SegmentPlotter.js';
import RelativeSegment from "@/data_classes/RelativeSegment";
import AbsoluteSegment from "@/data_classes/AbsoluteSegment";
import GridEventBus from '@/components/Map/GridEventBus.js';

export default {
  name: "Route",
  props: ['route', 'origin', 'rover', 'gridSquare'],
  computed: {
    routeName: function () {
      return this.route.routeName;
    },
  },
  mounted() {
    let comp = this;

    // When grid is mounted, draw ALL saved routes so that they
    // have the right transform
    //GridEventBus.$on("GRID_MOUNTED", function (payLoad) {
    if (this.rover.rover.xPosPx) { //Check that happens AFTER grid is ready
      comp.drawSegments(comp.route);
      if (comp.route.isVisible) {
          d3.select("#" + comp.route.routeName + "-Group")
            .style("display", "block")
        } else {
          d3.select("#" + comp.route.routeName + "-Group")
            .style("display", "none")
      }
    }

    //});

  },
  methods: {

    /**
     * Performs necessary computations to plot the line and point
     * for the route depending on the tab it is on.
     * 
     * Will draw the route if there is at least distance defined.
     * 
     * @param {object} route      Route object
    */
    drawSegments(route) {
      let roverTrans = this.convertCmToPx(this.rover.rover.xCmFromLander, this.rover.rover.yCmFromLander);

      for (let i = 0; i < route.segmentList.length; i++) {

        let segment = route.segmentList[i];

        // If first segment
        if (i === 0) {

          let {angle, startX, startY, endX, endY} = generateFirstSegmentVars(segment, 
                                                                this.rover.rover.xPosPx, 
                                                                this.rover.rover.yPosPx,
                                                                this.origin.origin.xPosPx,
                                                                this.origin.origin.yPosPx,
                                                                this.gridSquare.gridSquare.gridUnitCm,
                                                                this.gridSquare.gridSquare.gridUnitPx,
                                                                this.rover.rover.xCmFromLander,
                                                                this.rover.rover.yCmFromLander);
 
          let currRouteTransform = 
            d3.select("#visibleRoutes")
              .select("#"+route.routeName)
              .append('g')
              .attr("id", route.routeName+"-Group")
              .append('g')
              .attr("id", route.routeName+"-Segment0");

          plotNewSegment(currRouteTransform, route.routeName, 0, angle, startX, startY, endX, endY, false);

          // Calculate coords for relatives
          if (segment.constructor.name === "RelativeSegment") {
            calculateRelativeSegmentCoordinates(segment, 
                                                route.routeName, 
                                                0, 
                                                this.rover.rover.xPosPx - roverTrans.xPx, //Subtract by rover trans
                                                this.rover.rover.yPosPx - roverTrans.yPx, 
                                                this.gridSquare.gridSquare.gridUnitCm, 
                                                this.gridSquare.gridSquare.gridUnitPx);
          }

        // Appended segments
        } else {
          let {computedAngle, startX, startY, endX, endY} = generateAppendedSegmentVars(route, 
                                                                                        segment, 
                                                                                        i-1, 
                                                                                        this.gridSquare.gridSquare.gridUnitCm, 
                                                                                        this.gridSquare.gridSquare.gridUnitPx,
                                                                                        this.rover.rover.xPosPx,
                                                                                        this.rover.rover.yPosPx,
                                                                                        this.origin.origin.xPosPx,
                                                                                        this.origin.origin.yPosPx);

          let transform = d3.select("#"+route.routeName+"-Group")
                        .append('g')
                        .attr("id", route.routeName+"-Segment"+i);

          plotNewSegment(transform, route.routeName, i, computedAngle, startX, startY, endX, endY, false);

          // Calculate coords for relatives
          if (segment.constructor.name === "RelativeSegment") {
            calculateRelativeSegmentCoordinates(segment, 
                                                route.routeName, 
                                                i, 
                                                this.rover.rover.xPosPx - roverTrans.xPx, //Subtract by rover trans
                                                this.rover.rover.yPosPx - roverTrans.yPx, 
                                                this.gridSquare.gridSquare.gridUnitCm, 
                                                this.gridSquare.gridSquare.gridUnitPx);
          }
        }
      }
      
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
      let yPx = (yCm / this.gridSquare.gridSquare.gridUnitCm) * this.gridSquare.gridSquare.gridUnitPx;
      let xPx = (xCm / this.gridSquare.gridSquare.gridUnitCm) * this.gridSquare.gridSquare.gridUnitPx;

      // Invert sign since pos value means negative translation 
      // (moving up) from origin  
      if (yPx > 0) {
        yPx *= -1;
      }

      return {xPx: xPx, yPx: yPx};
    }
  }
}

</script>

<style lang="scss" scoped>

</style>
