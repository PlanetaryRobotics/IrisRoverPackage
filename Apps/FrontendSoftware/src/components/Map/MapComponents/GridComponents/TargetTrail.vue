<template>
    <svg id="targetTrail">
      <line />
      <circle id="outer"/>
      <circle id="inner" />
    </svg>
</template>

<script>

import { mapState, mapGetters } from 'vuex';
import * as d3 from "d3";

export default {
  name: "TargetTrail",
  props: ['origin', 'gridSquare'],
  data() {
    return {
      currTargetWaypointData: null
    }
  },
  computed: {
    ...mapGetters(['commandData', 'localizedData']),
    ...mapState({
      rover: (state) => state.MAP.roverMetadata
    })
  },
  watch: {
    commandData: {
      handler: function (newData) {
        if (newData.length === 0) {return; }

        // Get the last command if it has waypoint data
        let lastCommand = newData[newData.length-1].data;
        let targetWaypoint = lastCommand.waypointData !== null ? lastCommand.waypointData : null;
        
        // Waypoint exists and is not same as the prev one, then make
        // it the new target
        if (targetWaypoint && this.currTargetWaypointData !== targetWaypoint) {

          // If there was a prev segment already visited
          if (this.currTargetWaypointData) {
            this.setTravelledSegmentColor();
          }

          // Set the current segment
          this.currTargetWaypointData = targetWaypoint;
          this.updateTrail();
        }
      }, 
      deep:true,
      immediate: true
    },
  },
  methods: {
    setTravelledSegmentColor() {
      this.currTargetWaypointData.segment.setAsVisited();

      let container;

      // If was a circumnav's WP
      if (this.currTargetWaypointData.circumnavigation.index !== null) {
        let routeName = this.currTargetWaypointData.route.routeName;
        let segIndex = this.currTargetWaypointData.circumnavigation.index;
        let circumIndex = this.currTargetWaypointData.index;

        container = d3.select("#"+routeName + "-Segment"+ segIndex)
                          .select("#"+routeName + "-CircumSegment" + circumIndex);

      // Else was a regular WP
      } else {
        container = d3.select("#"+this.currTargetWaypointData.route.routeName + "-Segment"+(this.currTargetWaypointData.index));
      }
      
      container.select("circle").style("fill", "#21DF9A");
      container.select("line").style("stroke", "#21DF9A");
    },
    updateTrail() {
      //Container for the gradient
      var defs = d3.select("#targetTrail").append("defs");

      //Append a linear horizontal gradient
      var linearGradient = defs.append("linearGradient")
          .attr("id","animate-gradient") //unique id for reference
          .attr("x1","100%")
          .attr("y1","0%")
          .attr("x2","200%")
          .attr("y2","0%")
          //Make sure the areas before 0% and after 100% (along the x)
          //are a mirror image of the gradient and not filled with the
          //color at 0% and 100%
          .attr("spreadMethod", "reflect");

      //A color palette that is 4 colors
      //(the last 3 colors are the reverse of the start)
      var colours = ["#3200fc", "#8A6DFF", "#FFD039", "#8A6DFF", "#3200fc"];

      //Append the colors evenly along the gradient
      linearGradient.selectAll(".stop")
          .data(colours)
          .enter().append("stop")
          .attr("offset", function(d,i) { return i/(colours.length-1); })
          .attr("stop-color", function(d) { return d; });

      //Animate the 'x1' attribute from 0% to 100% in 7 seconds
      //And do this indefinitely
      linearGradient.append("animate")
          .attr("attributeName","x1")
          .attr("values","100%;0%")
          .attr("dur","2s")
          .attr("repeatCount","indefinite");

      //Animate the 'x2' attribute from 100% to 200% in 7 seconds
      linearGradient.append("animate")
          .attr("attributeName","x2")
          .attr("values","200%;100%")
          .attr("dur","2s")
          .attr("repeatCount","indefinite");

      // Get end coords
      let endX = this.currTargetWaypointData.segment.xPxCoordinate;
      let endY = this.currTargetWaypointData.segment.yPxCoordinate;

      // Set line
      d3.select("#targetTrail line")
        .attr("x1", this.rover.xPosPx)
        .attr("y1", this.rover.yPosPx)
        .attr("x2", endX)
        .attr("y2", endY)
        .style("stroke", "url(#animate-gradient)") 
        .style("stroke-width", "3px");

      // Set end circles
      d3.select("#targetTrail circle#inner")
        .attr("cx", endX)
        .attr("cy", endY)
        .attr("r", "4px")
        .style("fill", "#8A6DFF");

      d3.select("#targetTrail circle#outer")
        .attr("cx", endX)
        .attr("cy", endY)
        .attr("r", "7px")
        .style("stroke", "#8A6DFF")
        .style("fill", "black")
        .style("stroke-width","2px");
    },

  }
}

</script>

<style lang="scss" scoped>
  @import '@/styles/_colors.scss';

  .temp {
    fill: white;
  }

</style>
