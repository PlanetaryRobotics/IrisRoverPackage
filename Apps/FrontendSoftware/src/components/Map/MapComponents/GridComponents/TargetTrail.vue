<template>
    <svg id="targetTrail">
      <line />
      <circle id="outer"/>
      <circle id="inner" />
    </svg>
</template>

<script>

import { mapGetters } from 'vuex';
import * as d3 from "d3";

export default {
  name: "TargetTrail",
  props: ['origin', 'rover', 'gridSquare'],
  data() {
    return {
      currTargetWaypoint: null
    }
  },
  computed: {
    ...mapGetters(['commandData', 'localizedData']),
  },
  watch: {
    commandData: {
      handler: function (newData) {
        if (newData.length === 0) {return; }

        let targetWaypoint = newData[newData.length-1].data.waypointData.segment;
        
        if (this.currTargetWaypoint !== targetWaypoint) {
          this.currTargetWaypoint = targetWaypoint;
          this.updateTrail();
        }
      }, 
      deep:true,
      immediate: true
    },
  },
  methods: {

    updateTrail() {

      //Container for the gradient
      var defs = d3.select("#targetTrail").append("defs");
      debugger;
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

      // Set line
      d3.select("#targetTrail line")
        .attr("x1", this.rover.rover.xPosPx)
        .attr("y1", this.rover.rover.yPosPx)
        .attr("x2", this.currTargetWaypoint.xPxCoordinate)
        .attr("y2", this.currTargetWaypoint.yPxCoordinate)
        .style("stroke", "url(#animate-gradient)") 
        .style("stroke-width", "3px");

      // Set end circles
      d3.select("#targetTrail circle#inner")
        .attr("cx", this.currTargetWaypoint.xPxCoordinate)
        .attr("cy", this.currTargetWaypoint.yPxCoordinate)
        .attr("r", "3px")
        .style("fill", "#8A6DFF");

      d3.select("#targetTrail circle#outer")
        .attr("cx", this.currTargetWaypoint.xPxCoordinate)
        .attr("cy", this.currTargetWaypoint.yPxCoordinate)
        .attr("r", "6px")
        .style("stroke", "#8A6DFF")
        .style("fill", "black")
        .style("stroke-width","2px");
    },

    drawTrail(data) {
      let that = this;

      let valueline = d3.line()
          .x(function(d) { 
            return that.computeCmToPx("X", d.data.position[0]);
          })
          .y(function(d) { 
            return that.computeCmToPx("Y", d.data.position[1]);
          });

      d3.select("#localizedTrail .line") 
        .attr("d", valueline(data));

      d3.select("#localizedTrail .underLine")
        .attr("d", valueline(data));
    },

    plotPoint(point) {
      let diamond = d3.symbol().type(d3.symbolDiamond).size(30);
      let x = this.computeCmToPx("X", point.data.position[0]);
      let y = this.computeCmToPx("Y", point.data.position[1]);

      d3.select("#localizedTrail")
        .append("path")
        .attr("class", "diamond")
        .attr("d", diamond)
        .attr("transform", `translate(${x},${y})`)
        .style("stroke", "white")
        .style("stroke-width", 2)
        .style("fill", "#8A6DFF");
    },

    /**
     * Converts a cm units to pixel units
     * 
     * @param {string} type    "X" or "Y"
     * @param {number} cm      cm value
     * 
     * @return {number}        px value
    */
    computeCmToPx(type, cm) {
      let px = (cm / this.gridSquare.gridSquare.gridUnitCm) * this.gridSquare.gridSquare.gridUnitPx;

      if (type === "Y") {
        px *= -1;
        px += this.origin.origin.yPosPx;
      } else {
        px += this.origin.origin.xPosPx;
      }

      return px;
    }
  }
}

</script>

<style lang="scss" scoped>
  @import '@/styles/_colors.scss';

  .temp {
    fill: white;
  }

</style>
