<template>
    <g id="localizedTrail">
      <path class="line" d=""/>
      <!-- <text v-for="(ldata) in localizationData" v-bind:key="ldata.data.lookupID">
        {{ldata.data.position}}
      </text> -->
    </g>
</template>

<script>

import { mapGetters } from 'vuex';
import * as d3 from "d3";

export default {
  name: "LocalizedTrail",
  props: ['origin', 'rover', 'gridSquare'],
  computed: {
    ...mapGetters(['localizationData', 'framePoints']),
  },
  watch: {
    localizationData: {
      handler: function (newData) {
        this.drawTrail(newData);
      }, 
      deep:true,
      immediate: true
    },
    framePoints(data) {
      // The last point is the newest point to be plotted
      this.plotPoint(data[data.length-1]);
    }
  },
  methods: {
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

  #localizedTrail .line {
    stroke: $color-primary;
    stroke-width: 3;
    fill: none;
  }

  .temp {
    fill: white;
  }

</style>
