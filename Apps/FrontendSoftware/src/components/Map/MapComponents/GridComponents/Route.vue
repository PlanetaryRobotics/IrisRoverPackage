<template>
  <g
    :id="routeName"
    class="route"
  />
</template>

<script>

import * as d3 from 'd3';
import { plotNewSegment, getAbsoluteCoordinates } from '@/components/Map/Utility/SegmentPlotter.js';
import { getCircumnavLastPoint } from '@/components/Map/Utility/CircumnavPlotter.js';

export default {
    name: 'Route',
    props: ['route', 'origin', 'gridSquare'],


    computed: {
        routeName: function () {
            return this.route.routeName;
        },
    },


    watch: {
        origin: {
            deep: true,
            handler(){
                this.update();
            }
        },
        gridSquare: {
            deep: true,
            handler(){
                this.update();
            }
        }
    },


    mounted() {
        this.update();
    },


    methods: {
    /**
     * Update the Route Drawing.
     */
        update(){
            if (this.origin.xPosPx != undefined) { //Check that happens AFTER grid is ready
                let comp = this;

                comp.drawSegments(comp.route);
                if (comp.route.isVisible) {
                    d3.select('#' + comp.route.routeName + '-Group')
                        .style('display', 'block');
                } else {
                    d3.select('#' + comp.route.routeName + '-Group')
                        .style('display', 'none');
                }
            }
        },

        /**
     * Convert a {xCm, yCm} POSITION (incl. origin) to a Px position (incl. origin offset)
     */
        Cm2Px_Coordinate({xCm, yCm}){
            return {
                xPx: this.origin.xPosPx + xCm * this.gridSquare.gridUnitPx / this.gridSquare.gridUnitCm,
                yPx: this.origin.yPosPx - yCm * this.gridSquare.gridUnitPx / this.gridSquare.gridUnitCm
            };
        },

        /**
     * Performs necessary computations to plot the line and point
     * for the route depending on the tab it is on.
     * 
     * Will draw the route if there is at least distance defined.
     * 
     * @param {object} route      Route object
    */
        drawSegments(route) {

            for (let i = 0; i < route.segmentList.length; i++) {
 
                let segment = route.segmentList[i];

                // If first segment
                if (i === 0) {
                    if (segment.constructor.name === 'WaypointSegment') {
                        this.plotFirstSegment(route, segment);
                    } else {
                        this.plotFirstCircumnav(route, segment);
                    }
                    // Appended segments
                } else {
                    if (segment.constructor.name === 'WaypointSegment') {
                        this.plotNextSegment(route, segment, i);
                    } else {
                        this.plotNextCircumnav(route, segment, i);
                    }
                }
            }
        },

        plotFirstSegment(route, segment) {
            // End coords is from segment itself
            let {xPx: endX, yPx: endY} = this.Cm2Px_Coordinate({xCm: segment.xCmCoordinate, yCm: segment.yCmCoordinate});

            // Start coords is same as end for very first waypoint
            let startX = endX;
            let startY = endY;

            let angle = 0;

            let currRouteTransform = 
        d3.select('#visibleRoutes')
            .select('#'+route.routeName)
            .append('g')
            .attr('id', route.routeName+'-Group')
            .append('g')
            .attr('id', route.routeName+'-Segment0');

            plotNewSegment(currRouteTransform, route.routeName, 0, angle, startX, startY, endX, endY, false, segment.roverAngle, segment.state);
        },

        plotNextSegment(route, segment, i) {

            let lastCircle; 

            // Check if prev segment was a circumnav
            if (route.segmentList[i-1].constructor.name === 'Circumnavigation') {
                lastCircle = getCircumnavLastPoint(route, i-1);
                // Else just get last point in route
            } else {
                lastCircle = d3.select('#'+route.routeName+'-Segment'+(i-1))
                    .select('circle');
            }
                    
            // Need to get the transformed coordinates of the previous circle
            let centre = getAbsoluteCoordinates(lastCircle);

            // Set start coords
            let startX = Number(centre.x);
            let startY = Number(centre.y);

            // Set end coords
            let {xPx: endX, yPx: endY} = this.Cm2Px_Coordinate({xCm: segment.xCmCoordinate, yCm: segment.yCmCoordinate});
            let angle = 0;

            let transform = d3.select('#'+route.routeName+'-Group')
                .append('g')
                .attr('id', route.routeName+'-Segment'+i);

            plotNewSegment(transform, route.routeName, i, angle, startX, startY, endX, endY, false, segment.roverAngle, segment.state);
        },

        plotFirstCircumnav(route, circumnav) {

            let container = 
        d3.select('#visibleRoutes')
            .select('#'+route.routeName)
            .append('g')
            .attr('id', route.routeName+'-Group')
            .append('g')
            .attr('id', route.routeName+'-Segment0');

            for (let i = 0; i < circumnav.waypoints.length; i++) {

                let segment = circumnav.waypoints[i];
        
                let group = container.append('g').attr('id', route.routeName + '-CircumSegment' + i);

                // For the first point
                if (i === 0) {
                    // End coords is from segment itself
                    let {xPx: endX, yPx: endY} = this.Cm2Px_Coordinate({xCm: segment.xCmCoordinate, yCm: segment.yCmCoordinate});

                    // Start coords is same as end for very first waypoint
                    let startX = endX;
                    let startY = endY;

                    let angle = 0;
          
                    plotNewSegment(group, route.routeName, i, angle, startX, startY, endX, endY, false, segment.roverAngle, segment.state);
        
                    // Preceding segments
                } else {
                    // Get last point in route
                    let lastCircle = container.select('#'+route.routeName+'-CircumSegment'+(i-1))
                        .select('circle');
                        
                    // Need to get the transformed coordinates of the previous circle
                    let centre = getAbsoluteCoordinates(lastCircle);

                    // Set start coords
                    let startX = Number(centre.x);
                    let startY = Number(centre.y);

                    // Set end coords
                    let {xPx: endX, yPx: endY} = this.Cm2Px_Coordinate({xCm: segment.xCmCoordinate, yCm: segment.yCmCoordinate});
                    let angle = 0;

                    plotNewSegment(group, route.routeName+'-CircumSegment', i, angle, startX, startY, endX, endY, false, segment.roverAngle, segment.state);
                }
            }
        },

        plotNextCircumnav(route, circumnav, currIdx) {

            let container = d3.select('#'+route.routeName+'-Group')
                .append('g')
                .attr('id', route.routeName+'-Segment'+currIdx);
      
            for (let i = 0; i < circumnav.waypoints.length; i++) {

                let segment = circumnav.waypoints[i];
        
                let group = container.append('g').attr('id', route.routeName + '-CircumSegment' + i);

                let lastCircle;
                if (i === 0) {
                    // Get last point in prev circumnav
                    if (route.segmentList[currIdx-1].constructor.name === 'Circumnavigation') {
                        lastCircle = getCircumnavLastPoint(route, currIdx-1);
                        // Get point in prev segment
                    } else {
                        lastCircle = d3.select('#'+route.routeName+'-Segment'+(currIdx-1))
                            .select('circle');
                    }
                    // Get last point in current circumnav's 
                } else {
                    lastCircle = container.select('#'+route.routeName+'-CircumSegment'+(i-1))
                        .select('circle');
                }

                // Need to get the transformed coordinates of the previous circle
                let centre = getAbsoluteCoordinates(lastCircle);

                // Set start coords
                let startX = Number(centre.x);
                let startY = Number(centre.y);

                // Set end coords
                let {xPx: endX, yPx: endY} = this.Cm2Px_Coordinate({xCm: segment.xCmCoordinate, yCm: segment.yCmCoordinate});
                let angle = 0;

                plotNewSegment(group, route.routeName+'-CircumSegment', i, angle, startX, startY, endX, endY, false, segment.roverAngle, segment.state);
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
            let yPx = (yCm / this.gridSquare.gridUnitCm) * this.gridSquare.gridUnitPx;
            let xPx = (xCm / this.gridSquare.gridUnitCm) * this.gridSquare.gridUnitPx;

            // Invert sign since pos value means negative translation 
            // (moving up) from origin  
            if (yPx > 0) {
                yPx *= -1;
            }

            return {xPx: xPx, yPx: yPx};
        }
    }
};

</script>

<style lang="scss" scoped>
@import '@/styles/_typography.scss';
@import '@/styles/_colors.scss';

</style>
