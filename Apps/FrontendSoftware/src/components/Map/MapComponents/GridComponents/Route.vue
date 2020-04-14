<template>
    <g :id = "routeName" class="route">
    </g>
</template>

<script>

import * as d3 from "d3";
import { plotNewSegment, 
         generateAppendedSegmentVars, 
         calculateCmCoordinatesForSegment } from '@/components/Map/Utility/SegmentPlotter.js';
import { toggleModal } from '@/components/Map/Utility/ModalPlotter.js';

export default {
  name: "Route",
  props: ['route', 'origin', 'rover', 'gridSquare'],
  data() {
    return {
      
    }
  },
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

          // Start coords is rover position
          let startX = this.rover.rover.xPosPx;
          let startY = this.rover.rover.yPosPx;

          // End coords is from segment itself
          let endX = segment.xPxCoordinate;
          let endY = segment.yPxCoordinate;
          let angle = 0;

          let currRouteTransform = 
            d3.select("#visibleRoutes")
              .select("#"+route.routeName)
              .append('g')
              .attr("id", route.routeName+"-Group")
              .append('g')
              .attr("id", route.routeName+"-Segment0")

          let circleCoords = plotNewSegment(currRouteTransform, route.routeName, 0, angle, startX, startY, endX, endY, false);
          
          // Set up modal once have coords w/ rotation
          currRouteTransform.on("click", function() {
                              toggleModal(this, route, 0, circleCoords.x, circleCoords.y); 
                            });

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
   
          let circleCoords = plotNewSegment(transform, route.routeName, i, computedAngle, startX, startY, endX, endY, false);

          // Set up modal once have coords w/ rotation
          transform.on("click", function() {
                        toggleModal(this, route, i, circleCoords.x, circleCoords.y); 
                    });

          // Calculate coords for relatives
          if (segment.constructor.name === "RelativeSegment") {
            calculateCmCoordinatesForSegment(segment, 
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
@import '@/styles/_typography.scss';
@import '@/styles/_colors.scss';

</style>
